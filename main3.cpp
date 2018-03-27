/*=============================================================================
 PIC32MM USB Curiosity Board (PIC32MM0256GPM064)

 testing - using C++

 http://www.microchip.com/wwwproducts/en/PIC32MM0256GPM064
 http://ww1.microchip.com/downloads/en/DeviceDoc/60001387c.pdf

 =============================================================================*/


#include <cstdint>
#include <stdio.h>

#include "Pins.hpp"
#include "Wdt.hpp"
#include "Osc.hpp"
#include "Delay.hpp"
#include "Resets.hpp"
#include "Ccp.hpp"
#include "Uart.hpp"
#include "Cp0.hpp"
#include "Rtcc.hpp"
#include "Irq.hpp"


//svg colors for rgb led
const uint8_t svg[][3]{
/*blue*/ {0,0,255},
/*darkgoldenrod*/ {184,134,11},
/*darkgreen*/ {0,100,0},
/*darkmagenta*/ {139,0,139},
/*darkred*/ {139,0,0},
/*darkseagreen*/ {143,188,143},
/*darkviolet*/ {148,0,211},
/*deeppink*/ {255,20,147},
/*hotpink*/ {255,105,180},
/*lime*/ {0,255,0},
/*magenta*/ {255,0,255},
/*navy*/ {0,0,128},
/*orange*/ {255,165,0},
/*purple*/ {128,0,128},
/*red*/ {255,0,0},
/*yellow*/ {255,255,0},
};


Uart info{Uart::UART2, Pins::C6, Pins::C7, 230400};


//printf - use replacement putc
//will use $ for trigger to print ansi colors (use $$ if want $ character)
//printf("this is $1red $7white and I have $$1");
extern "C" void _mon_putc(char c){
    static bool trigger = false;
    if(trigger){
        trigger = false;
        if(c >= '0' && c <= '7'){
            info.puts("\033[3");    //ansi color start
            info.putchar(c);        //plus color
            c = 'm';                //below will output this
        }
        info.putchar(c);            //'m' from above, or regular char after '$'
        return;
    }
    //not triggered
    if(c == '$') trigger = true;//trigger char
    else info.putchar(c);       //regular char
}

void cls(){ info.putchar(12); }
void cursor(bool tf){ printf("\033[?25%c", tf ? 'h' : 'l'); }
void ansi_reset(){ printf("\033[0m"); }


//rgb led's struct, use pwm for brightness
//loop through svg colors by regularly calling update()
struct Rgb {

    Rgb(){
        //init pwm via loop- R,G use OCxB, B uses OCxE
        for(auto i = 0; i < 3; i++){
            m_ccp[i].mode(m_ccp[i].DEPWM16); //dual edge pwm 16bit
            m_ccp[i].compa(0);
            m_ccp[i].compb(0);
            m_ccp[i].out_pins(i == 2 ? Ccp::OCE : Ccp::OCB);
            m_ccp[i].on(true);
        }
    }

    //cycle through svg colors
    void update(){
        if(not m_delay.expired()) return;

        uint16_t t = m_delay_long;
        for(uint8_t i = 0; i < 3; i++){
            uint16_t v = m_ccp[i].compb();
            uint16_t s = svg[m_idx][i]<<8;
            if(v == s) continue;
            t = m_delay_short;
            if(v < s) v += 256; else v -= 256;
            m_ccp[i].compb(v);
        }
        m_delay.set_ms(t);
        if(t == m_delay_short) return;

        Rtcc::datetime_t dt = Rtcc::datetime();

        printf("$7color[$3%02d$7]: $2%03d.%03d.%03d$7",
            m_idx,m_ccp[0].compb()>>8,m_ccp[1].compb()>>8,m_ccp[2].compb()>>8);
        printf(" CP0 Count: $1%010u$7", Cp0::count());
        printf(" now: $5%02d-%02d-%04d %02d:%02d:%02d %s$7\r\n",
                dt.month, dt.day, dt.year+2000, dt.hour12, dt.minute, dt.second,
                dt.pm ? "PM" : "AM");

        if(++m_idx >= sizeof(svg)/sizeof(svg[0])) m_idx = 0;
    };

    private:

    static const uint16_t m_delay_short{10};
    static const uint16_t m_delay_long{1000};
    uint8_t m_idx{0};
    //pwm to rgb pins
    //mccp 1-3 pwm to rgb led's
    Ccp m_ccp[3]{ Ccp::CCP1, Ccp::CCP2, Ccp::CCP3 };

    Pins m_ledR{Pins::D1, Pins::OUT},
         m_ledG{Pins::C3, Pins::OUT},
         m_ledB{Pins::C15, Pins::OUT};

    Delay m_delay;

};



//alternate led/led2 at rate determined by pot via adc
//if very low value, turn off led's
struct Led12 {

    void update(){
        if(not m_delay.expired()) return;
        uint16_t t = m_pot.adcval()>>2; //(0-4096 -> 0-256)
        if(t < 100){
            t = 100;
            m_led1.off();
            m_led2.off();
        } else {
            m_led1_state = not m_led1_state;
            m_led1.latval(m_led1_state);
            m_led2.latval(not m_led1_state); //always opposite
        }
        m_delay.set_ms(t);
    };

    private:

    Pins m_pot{Pins::AN14}; //check pot val via adc
    Pins m_led1{Pins::D3, Pins::OUT};
    Pins m_led2{Pins::C13, Pins::OUT};
    bool m_led1_state{false};
    Delay m_delay;

};

extern "C" {
#include "usb.h"
}


int main()
{



    //just get/store resets cause (not used here,though)
    Resets::cause();

    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);       //8*12/4 = 24MHz
    osc.sosc(true);                         //enable sosc if not already
    osc.tun_auto(true);                     //let sosc tune frc

    Rtcc::datetime_t dt = Rtcc::datetime();
    if(dt.year == 0) Rtcc::datetime( { 18, 3, 16, 0, 10, 32, 0} );

    Rtcc::on(true);

    info.on(true);
    cls(); //cls
    cursor(false); //hide cursor


//trying MCHP usb code
Delay::wait_s(2);
Irq::init(Irq::USB, 1, 0, false);
USBDeviceInit();
USBDeviceAttach();
Irq::global(true);
#include <stdio.h>



    Rgb rgb;
    Led12 led12;
    Delay dly;
    dly.set_ms(100);

    for(uint32_t i = 0;;i++){
        Wdt::reset(), led12.update(), rgb.update();

        if(USBUSARTIsTxTrfReady() && dly.expired())
        {
            char data[32];
            snprintf(data, 32, "%08X ",  USBGet1msTickCount());
            putsUSBUSART(data);
            dly.restart();
        }
        CDCTxService();
    }
}

