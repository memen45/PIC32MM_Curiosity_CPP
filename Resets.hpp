#pragma once

#include "Syskey.hpp"

/*=============================================================================
 Resets functions
=============================================================================*/

class Resets {

    public:

        enum CAUSE : uint8_t
        {
            EXTR = 1<<7,
            SWR = 1<<6,
            //1<<5 none- reads 0
            WDTO = 1<<4,
            SLEEP = 1<<3,
            IDLE = 1<<2,
            BOR = 1<<1,
            POR = 1<<0
        };

        //RCON
        static CAUSE cause( void );
        static bool config_err( void );
        //RSWRST
        static void swreset( void );
        //RNMICON
        static bool nmi_wdt( void );
        static bool nmi_sw( void );
        static bool nmi_gen( void );
        static bool nmi_clkf( void );
        static bool nmi_wdts( void );
        static void nmi_wdtcount( uint16_t );
        static void nmi_wdtclr( void );
        //PWRCON
        static void bor( bool );
        static void retention( bool );
        static void vreg_standby( bool );

    private:

        enum {
        RCON = 0xBF8026E0,
            PORIO = 1<<31, PORCORE = 1<<30,
            BCFGERR = 1<<27, BCFGFAIL = 1<<26, CMR = 1<<9,
        RSWRST = 0xBF8026F0,
            SWRST = 1,
        RNMICON = 0xBF802700,
            WDTR = 1<<24, SWNMI = 1<<23, GNMI = 1<<19, CF = 1<<17, WDTS = 1<<16,

        PWRCON = 0xBF802710,
            SBOREN = 1<<2, RETEN = 1<<0, VREGS = 1<<0
    };
};

/*=============================================================================
 all functions inline
=============================================================================*/
//save rcon on boot (only one time)
static uint32_t boot_flags;

//RCON
//save flags, clear flags, return reset cause
Resets::CAUSE Resets::cause( void )
{
    //in case called >1 time, save flags only first time
    //boot_flags var will be 0 on any reset as c runtime will clear
    //before this function can run
    if( !boot_flags ){
        boot_flags = Reg::val( RCON );  //save
        Reg::val( RCON, 0 );            //then clear all flags
    }
    //check for por first
    if( boot_flags & (PORIO|PORCORE|BOR|POR) ) return POR;
    //then go through flags high bits to low bits
    //(sleep is before idle, so will not get false flag
    // because sleep also has idle set)
    uint8_t ret = EXTR;
    for( ; ret > POR; ret >>= 1 ){
        if( boot_flags & ret ) break;
    }
    return (CAUSE)ret;
}


bool Resets::config_err( void )
{
    return boot_flags & (BCFGERR|BCFGFAIL|CMR);
}

//RSWRST

//library version (uses asm, is ~30bytes smaller)
//    extern "C" {
//    void Resets::swreset(){
//        void __pic32_software_reset();  //declare
//        __pic32_software_reset();       //do
//    }
//    } //extern C

void Resets::swreset( void )
{
    Syskey::unlock();
    Reg::set( RSWRST, SWRST );
    Reg::val( RSWRST );
    //resets after read
}

//RNMICON
bool Resets::nmi_wdt( void ){            return Reg::is_set( RNMICON, WDTR ); }
bool Resets::nmi_sw( void ){             return Reg::is_set( RNMICON, SWNMI ); }
bool Resets::nmi_gen( void ){            return Reg::is_set( RNMICON, GNMI ); }
bool Resets::nmi_clkf( void ){           return Reg::is_set( RNMICON, CF ); }
bool Resets::nmi_wdts( void ){           return Reg::is_set( RNMICON, WDTS ); }
void Resets::nmi_wdtcount( uint16_t v ){        Reg::val16( RNMICON, v ); }
void Resets::nmi_wdtclr( void ){                Reg::clr( RNMICON, WDTR ); }

//PWRCON
void Resets::bor( bool tf ){
    Syskey::unlock();
    Reg::set( PWRCON, SBOREN, tf );
    Syskey::lock();
}
void Resets::retention( bool tf ){
    Syskey::unlock();
    Reg::set( PWRCON,RETEN, tf );
    Syskey::lock();
}
void Resets::vreg_standby( bool tf ){
    Syskey::unlock();
    Reg::set( PWRCON,VREGS, tf );
    Syskey::lock();
}