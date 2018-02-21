#include "Rtcc.hpp"
#include "Osc.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum : uint32_t {
    RTCCON1 = 0xBF800000,
        ALARMEN = 31,
        CHIME = 30,
        AMASK_SHIFT = 24, AMASK_MASK = 15,
        ALMRPT_SHIFT = 16, ALMRPT_MASK = 255,
        ON = 15,
        WRLOCK = 11,
        PINON = 7,
        OUTSEL_SHIFT = 4, OUTSEL_MASK = 7,
    RTCCON2 = 0xBF800010,
        FRDIV_SHIFT = 11, FRDIV_MASK = 31,
        PS_SHIFT = 4, PS_MASK = 3,
        CLKSEL_SHIFT = 9, CLKSEL_MASK = 3,
    RTCSTAT = 0xBF800030,
        ALMSTAT = 5,
        SYSNCSTAT = 2,
        ALMSYNCSTAT = 1,
        HALFSTAT = 0,
    RTCTIME = 0xBF800040,
    RTCDATE = 0xBF800050,
    ALMTIME = 0xBF800060,
    ALMDATE = 0xBF800070,
};

//clock divide precomputed for 32khz (prescale default 1:1)
//need 2Hz
enum : uint16_t { CLK_DIV_32KHZ = 0x3FFF };


//=============================================================================
    void        Rtcc::alarm             (bool tf)
//=============================================================================
{
    conset(RTCCON1, 1<<ALARMEN, tf);
}

//=============================================================================
    void        Rtcc::chime             (bool tf)
//=============================================================================
{
    conset(RTCCON1, 1<<CHIME, tf);
}

//=============================================================================
    void        Rtcc::alarm_interval    (AMASK e)
//=============================================================================
{
    conset(RTCCON1, AMASK_MASK<<AMASK_SHIFT, 0);
    conset(RTCCON1, e<<AMASK_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::alarm_repeat      (uint8_t v)
//=============================================================================
{
    conset(RTCCON1, ALMRPT_MASK<<ALMRPT_SHIFT, 0);
    conset(RTCCON1, v<<ALMRPT_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::on                (bool tf)
//=============================================================================
{
    if(tf and Reg::val16(RTCCON2 + 2)){     //div not set, so
        clk_div(CLK_DIV_32KHZ);             //init ourselves
        if(Osc::sosc()) clk_src(SOSC);      //use sosc if on
        else clk_src(LPRC);                 //else use lprc
        clk_pre(PRE1);                      //should be already
    }
    conset(RTCCON1, 1<<ON, tf);
}

//=============================================================================
    void        Rtcc::out_pin           (OUTSEL v)
//=============================================================================
{
    if(v != OFF){
        conset(RTCCON1, OUTSEL_MASK<<OUTSEL_SHIFT, 0);
        conset(RTCCON1, v<<OUTSEL_SHIFT, 1);
    }
    conset(RTCCON1, 1<<PINON, v != OFF);
}

//=============================================================================
    void        Rtcc::clk_div           (uint16_t v)
//=============================================================================
{
    unlock();
    Reg::val(RTCCON2+2, v);
    lock();
}

//=============================================================================
    void        Rtcc::clk_frdiv         (uint8_t v)
//=============================================================================
{
    conset(RTCCON2, FRDIV_MASK<<FRDIV_SHIFT, 0);
    conset(RTCCON2, (v bitand FRDIV_MASK)<<FRDIV_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::clk_pre           (PS e)
//=============================================================================
{
    conset(RTCCON2, PS_MASK<<PS_SHIFT, 0);
    conset(RTCCON2, e<<PS_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::clk_src           (CLKSEL e)
//=============================================================================
{
    if(e == SOSC) Osc::sosc(true);
    conset(RTCCON2, CLKSEL_MASK<<CLKSEL_SHIFT, 0);
    conset(RTCCON2, e<<CLKSEL_SHIFT, 1);
}

//=============================================================================
    bool        Rtcc::alarm_evt         ()
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<ALMSTAT);
}

//=============================================================================
    bool        Rtcc::time_busy         ()
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<SYSNCSTAT);
}

//=============================================================================
    bool        Rtcc::alarm_busy        ()
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<ALMSYNCSTAT);
}

//=============================================================================
    bool        Rtcc::half_sec          ()
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<HALFSTAT);
}

//=============================================================================
    auto        Rtcc::time              () -> time_t
//=============================================================================
{
    time_t t;
    t.w = Reg::val(RTCTIME);
    return t;
}

//=============================================================================
    auto        Rtcc::date              () -> date_t
//=============================================================================
{
    date_t d;
    d.w = Reg::val(RTCDATE);
    return d;
}

//=============================================================================
    auto        Rtcc::alarm_time        () -> time_t
//=============================================================================
{
    time_t t;
    t.w = Reg::val(ALMTIME);
    return t;
}

//=============================================================================
    auto        Rtcc::alarm_date        () -> date_t
//=============================================================================
{
    date_t d;
    d.w = Reg::val(ALMDATE);
    return d;
}

//=============================================================================
    void        Rtcc::time              (time_t v)
//=============================================================================
{
    conval(RTCTIME, v.w);
} //wrlock
//=============================================================================
    void        Rtcc::date              (date_t v)
//=============================================================================
{
    conval(RTCTIME, v.w);
} //wrlock
//=============================================================================
    void        Rtcc::alarm_time        (time_t v)
//=============================================================================
{
    Reg::val(ALMTIME, v.w);
}

//=============================================================================
    void        Rtcc::alarm_date        (date_t v)
//=============================================================================
{
    Reg::val(ALMTIME, v.w);
}

//RTCCON1 lock off by default, these functions will lock RTCCON1 when done
//private functions
//=============================================================================
    void        Rtcc::unlock            ()
//=============================================================================
{
    Sys::unlock();
    Reg::clrbit(RTCCON1, 1<<WRLOCK);
}

//=============================================================================
    void        Rtcc::lock              ()
//=============================================================================
{
    Reg::setbit(RTCCON1, 1<<WRLOCK);
    Sys::lock();
}

//=============================================================================
    void        Rtcc::conset            (uint32_t addr, uint32_t v, bool tf)
//=============================================================================
{
    unlock();
    Reg::setbit(addr, v, tf);
    lock();
}

//=============================================================================
    void        Rtcc::conval            (uint32_t addr, uint32_t v)
//=============================================================================
{
    unlock();
    Reg::val(addr, v);
    lock();
}
