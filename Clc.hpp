#pragma once

/*=============================================================================
 CLC1-4 functions
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Clc {

    //instantiate Clc with clc number
    enum CLC { CLC1 = 0, CLC2, CLC3, CLC4 };
    //logic function
    enum MODE : uint8_t { ANDOR = 0, ORXOR, AND, SR, DSR, DR, JKR, LSR };
    //gate polarity
    enum GXPOL { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };

    //public functions
    void    gate_inv    (GXPOL, bool);
    void    on          (bool);
    void    stop_idle   (bool);
    void    intp        (bool);
    void    intn        (bool);
    void    out         (bool);
    bool    out         ();
    void    out_inv     (bool);
    void    mode        (MODE e);
    void    in_sel      (uint8_t, uint8_t);
    void    in_sel      (uint32_t);
    void    gate_sel    (uint8_t, uint8_t);
    void    gate_sel    (uint32_t);

    //constructor
    constexpr Clc(CLC);

    private:

    static Reg r;

    enum {
        CLC1CON = 0xBF802480, CLCXCON_SPACING = 32, //spacing in words
        SEL_OFFSET = 4, GLS_OFFSET = 8, //offsets in words
        ON = 1<<15, SIDL = 1<<13, INTP = 1<<11 , INTN = 1<<10,
        LCOE = 1<<7, LCOUT = 1<<6, LCPOL = 1<<5
    };

    //private vars
    volatile uint32_t* m_pt;
};


/*=============================================================================
 all functions inline
=============================================================================*/

constexpr Clc::Clc(CLC e)
    : m_pt((volatile uint32_t*)CLC1CON+(e*CLCXCON_SPACING)){}

void Clc::gate_inv(GXPOL e, bool tf){
    r.setbit(m_pt, e, tf);
}
void Clc::on(bool tf){
    r.setbit(m_pt, ON, tf);
}
void Clc::stop_idle(bool tf){
    r.setbit(m_pt, SIDL, tf);
}
void Clc::intp(bool tf){
    r.setbit(m_pt, INTP, tf);
}
void Clc::intn(bool tf){
    r.setbit(m_pt, INTN, tf);
}
void Clc::out(bool tf){
    r.setbit(m_pt, LCOE, tf);
}
bool Clc::out(){
    return r.anybit(m_pt, LCOUT);
}
void Clc::out_inv(bool tf){
    r.setbit(m_pt, LCPOL, tf);
}
void Clc::mode(MODE e){
    r.setbit(m_pt, LSR, 0);
    r.setbit(m_pt, e);
}
//input select, n = 1-4, v = 0-7 (invalid args masked to good vals)
void Clc::in_sel(uint8_t n, uint8_t v){
    n -= 1; n &= 3; n <<= 2; v &= 7;
    r.setbit(m_pt+SEL_OFFSET, 7<<n, 0);
    r.setbit(m_pt+SEL_OFFSET, v<<n);
}
//or all in in shot with precomputed value
void Clc::in_sel(uint32_t v){
    r.val(m_pt+SEL_OFFSET, v);
}
//gate select, n = 1-4 (invalid gate masked to good gate)
void Clc::gate_sel(uint8_t n, uint8_t v){
    n -= 1; n &= 3; n <<= 3;
    r.setbit(m_pt+GLS_OFFSET, 15<<n, 0);
    r.setbit(m_pt+GLS_OFFSET, v<<n);
}
//or all in in shot with precomputed value
void Clc::gate_sel(uint32_t v){
    r.val(m_pt+GLS_OFFSET, v);
}
