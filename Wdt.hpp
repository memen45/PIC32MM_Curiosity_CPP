#pragma once

/*=============================================================================
 Watchdog timer functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Wdt {

    //public functions
    static void     reset       ();
    static void     on          (bool);
    static void     window_on   (bool);

    private:

    static Reg r;

    enum {
        WDTCON = 0xBF803990, CLRKEY = 0x5743,
        ON = 1<<15, WINEN = 1<<0
        //RUNDIV, SLPDIV, CLKSEL are read-only and are set in config bits
        //if need to read them, can make your own read function(s) here or use
        //uint32_t val = Reg::val(Wdt::WDTCON); -then decipher on your own
    };
};

/*=============================================================================
 all functions inline
=============================================================================*/

//16bit write to upper 16bits of WDTCON
//add 2 bytes to base to get the correct address for the 16bit write
//(using enum values for WDTCON, so is normal byte addition)
void Wdt::reset(){ r.val16(WDTCON+2, CLRKEY); }
void Wdt::on(bool tf){ r.set(WDTCON, ON, tf); }
void Wdt::window_on(bool tf){ r.set(WDTCON, WINEN, tf); }