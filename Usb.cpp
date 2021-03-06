//USB peripheral - PIC32MM0256GPM064

#include <cstdint>
#include <cstring>
#include "Usb.hpp"
#include "Pins.hpp"

//registers - all registers use only first 8bits
enum {
U1OTGIR = 0xBF885040,   //no SET, INV - all bits write-1-to-clear
U1OTGIE = 0xBF885050,
U1OTGSTAT = 0xBF885060, //no SET, INV, CLR
U1OTGCON = 0xBF885070,
U1PWRC = 0xBF885080,
U1IR = 0xBF885200,      //no SET, INV
U1IE = 0xBF885210,
U1EIR = 0xBF885220,     //no SET, INV
U1EIE = 0xBF885230,
U1STAT = 0xBF885240,    //no SET, INV, CLR
    ENDPT_SHIFT = 4, ENDPT_MASK = 15,
    DIR = 3,
    PPBI = 2,
U1CON = 0xBF885250,
U1ADDR = 0xBF885260,
    LSEN = 7,
U1BDTP1 = 0xBF885270,
U1FRML = 0xBF885280,    //no SET, INV, CLR
U1FRMH = 0xBF885290,    //no SET, INV, CLR
U1TOK = 0xBF8852A0,
U1SOF = 0xBF8852B0,
U1BDTP2 = 0xBF8852C0,
U1BDTP3 = 0xBF8852D0,
U1CNFG1 = 0xBF8852E0,
U1EP0 = 0xBF885300, U1EP_SPACING = 0x10
};

//class var
//=============================================================================
volatile Usb::bdt_t Usb::bdt_table[][2][2] = {0};

//-----------------------------------------------------------------private-----
//Pins vbus_pin(Usb::vbus_pin_n, Pins::INPD);

//=============================================================================
            auto Usb::
bdt_init    () -> void
            {
            memset((void*)&bdt_table, 0, sizeof(bdt_table));
            bdt_addr((uint32_t)bdt_table);
            }

//=============================================================================
            auto Usb::
vbus_ison   () -> bool
            {
            //return vbus_pin.ison();
                return true;
            }

//U1OTGIR/IE, U1IR/IE, U1EIR/IE
//=============================================================================
            auto Usb::
flags       () -> uint32_t
            {
            return  val8(U1OTGIR)<<16 bitor
                    val8(U1EIR)<<8 bitor
                    val8(U1IR);
            }

//=============================================================================
            auto Usb::
flag        (FLAGS e) -> bool
            {
            return flags() bitand e;
            }

//=============================================================================
            auto Usb::
flags_clr   (uint32_t v) -> void
            {
            val(U1OTGIR, (uint8_t)(v>>16));
            val(U1EIR, (uint8_t)(v>>8));
            val(U1IR, (uint8_t)v);
            }

//=============================================================================
            auto Usb::
irqs        () -> uint32_t
            {
            return  val8(U1OTGIE)<<16 bitor
                    val8(U1EIE)<<8 bitor
                    val8(U1IE);
            }

//=============================================================================
            auto Usb::
irq         (FLAGS e) -> bool
            {
            return irqs() bitand e;
            }

//=============================================================================
            auto Usb::
irqs        (uint32_t v) -> void
            {
            val(U1OTGIE, (uint8_t)(v>>16));
            val(U1EIE, (uint8_t)(v>>8));
            val(U1IE, (uint8_t)v);
            }

//=============================================================================
            auto Usb::
irq         (FLAGS e, bool tf) -> void
            {
            if(e > (1<<15)) setbit(U1OTGIE, e>>16, tf);
            else if(e > (1<<7))setbit(U1EIE, e>>8, tf);
            else setbit(U1IE, e, tf);
            }


//U1OTGSTAT
//=============================================================================
            auto Usb::
otg_stat    () -> uint32_t
            {
            return val8(U1OTGSTAT)<<16;
            }

//=============================================================================
            auto Usb::
otg_stat    (FLAGS e) -> bool
            {
            return anybit(U1OTGSTAT, e>>16);
            }

//U1OTGCON
//=============================================================================
            auto Usb::
otg         (OTG e, bool tf) -> void
            {
            setbit(U1OTGCON, e, tf);
            }

//=============================================================================
            auto Usb::
otg         (uint8_t v) -> void
            {
            val(U1OTGCON, v);
            }

//U1PWRC
//=============================================================================
            auto Usb::
power       (POWER e) -> bool
            {
            return anybit(U1PWRC, e);
            }

//=============================================================================
            auto Usb::
power       (POWER e, bool tf) -> void
            {
            setbit(U1PWRC, e, tf);
            if(e == USBPWR and tf) bdt_init();
            }

//U1STAT
//=============================================================================
            auto Usb::
stat        () -> uint8_t
            {
            return val8(U1STAT)>>2;
            }

//U1CON
//=============================================================================
            auto Usb::
control     (CONTROL e) -> bool
            {
            return anybit(U1CON, e);
            }

//=============================================================================
            auto Usb::
control     (CONTROL e, bool tf) -> void
            {
            setbit(U1CON, e, tf);
            }

//=============================================================================
            auto Usb::
control     (uint8_t v) -> void
            {
            val(U1CON, v);
            }

//U1ADDR
//=============================================================================
            auto Usb::
low_speed   (bool tf) -> void
            {
            setbit(U1ADDR, 1<<LSEN, tf);
            }

//=============================================================================
            auto Usb::
dev_addr    () -> uint8_t
            {
            return val8(U1ADDR) bitand 127;
            }

//=============================================================================
            auto Usb::
dev_addr    (uint8_t v) -> void
            {
            clrbit(U1ADDR, 127);
            setbit(U1ADDR, v bitand 127);
            }

//U1FRML,H
//=============================================================================
            auto Usb::
frame       () -> uint16_t
            {
            return (val8(U1FRMH)<<8) bitor val8(U1FRML);
            }

//U1TOK
//=============================================================================
            auto Usb::
tok_pid     (TOKPID e) -> void
            {
            clrbit(U1TOK, 15<<4);
            setbit(U1TOK, e);
            }

//=============================================================================
            auto Usb::
tok_ep      (uint8_t v) -> void
            {
            clrbit(U1TOK, 15);
            setbit(U1TOK, v bitand 15);
            }

//U1SOF
//=============================================================================
            auto Usb::
sof_cnt     (SOFVALS e) -> void
            {
            val(U1SOF, e);
            }

//U1BDTP1,2,3
//=============================================================================
            auto Usb::
bdt_addr    (uint32_t v) -> void
            {
            v = k2phys(v); //physical address
            val(U1BDTP1, (uint8_t)(v>>8)); //512byte aligned (bit0 unused)
            val(U1BDTP2, (uint8_t)(v>>16));
            val(U1BDTP3, (uint8_t)(v>>24));
            }

//=============================================================================
            auto Usb::
bdt_addr    (uint8_t n, bool trx, bool eveodd) -> volatile bdt_t*
            {
            if(n > max_endpoint) return 0;      //invalid endpoint
            uint32_t v = p2kseg1(          //check if bdt table address set
                val8(U1BDTP3)<<24 bitor
                val8(U1BDTP2)<<16 bitor
                val8(U1BDTP1)<<8 );
            if(v != (uint32_t)bdt_table) return 0;  //something wrong
            return &bdt_table[n][trx][eveodd];
            }

//U1CNFG1
//=============================================================================
            auto Usb::
config      (CONFIG e, bool tf) -> void
            {
            setbit(U1CNFG1, e, tf);
            }

//=============================================================================
            auto Usb::
config      (CONFIG e) -> bool
            {
            return anybit(U1CNFG1, e);
            }

//=============================================================================
            auto Usb::
config      (uint8_t v) -> void
            {
            val(U1CNFG1, v);
            }

//U1EP0-15
//=============================================================================
            auto Usb::
epcontrol   (uint8_t n, EPCTRL e, bool tf) -> void
            {
            n and_eq 15;
            setbit(U1EP0+(n * U1EP_SPACING), e, tf);
            }

//=============================================================================
            auto Usb::
epcontrol   (uint8_t n, EPCTRL e) -> bool
            {
            n and_eq 15;
            return anybit(U1EP0+(n * U1EP_SPACING), e);
            }

//=============================================================================
            auto Usb::
epcontrol   (uint8_t n, uint8_t v) -> void
            {
            n and_eq 15;
            val(U1EP0+(n * U1EP_SPACING), v);
            }

//=============================================================================
            auto Usb::
epcontrol   (uint8_t n) -> uint8_t
            {
            n and_eq 15;
            return val8(U1EP0+(n * U1EP_SPACING));
            }

//regs to reset state
//=============================================================================
            auto Usb::
reset       () -> void
            {
            irqs(0);
            val(U1CON, 0);     //usben
            val(U1PWRC, 0);    //usbpwr
            while(power(USBBUSY));
            flags_clr(0xFFFFFF);
            val(U1ADDR, 0);
            val(U1TOK, 0);
            val(U1SOF, 0);
            val(U1OTGCON, 0);
            val(U1CNFG1, 1);
            for(auto i = 0; i < 16; i++) epcontrol(i, 0);
            val(U1BDTP1, 0);
            val(U1BDTP2, 0);
            val(U1BDTP3, 0);
            }
