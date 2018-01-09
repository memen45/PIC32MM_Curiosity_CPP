#include "Timer23.hpp"

/*=============================================================================
 Timer23 functions
=============================================================================*/
void Timer23::count(uint32_t n) const {
    m_tmrx = n;
    if(r.anybit(m_txcon, T32)){
        *((volatile uint32_t*)m_tmrx+TMR23_SPACING) = n >> 16;
    }
}
uint32_t Timer23::count() const {
    uint32_t ret = 0;
    if(r.anybit(m_txcon, T32)){
        ret = *((volatile uint32_t*)m_tmrx+TMR23_SPACING) << 16;
    }
    return ret | m_tmrx;
}
void Timer23::period(uint32_t n) const {
    m_prx = n;
    if(r.anybit(m_txcon, T32)){
        *((volatile uint32_t*)m_prx+TMR23_SPACING) = n >> 16;
    }
}
uint32_t Timer23::period() const {
    uint32_t ret = 0;
    if(r.anybit(m_txcon, T32)){
        ret = *((volatile uint32_t*)m_prx+TMR23_SPACING) << 16;
    }
    return ret | m_prx;
}
void Timer23::on(bool tf) const {
    r.setbit(m_txcon, ON, tf);
}
void Timer23::stop_idle(bool tf) const {
    r.setbit(m_txcon, SIDL, tf);
}
void Timer23::gate(bool tf) const {
    r.setbit(m_txcon, TGATE, tf);
}
void Timer23::prescale(PRESCALE e) const {
    r.clrbit(m_txcon, TCKPS_CLR<<TCKPS_SHIFT);
    r.setbit(m_txcon, e<<TCKPS_SHIFT);
}
void Timer23::mode32(bool tf) const {
    r.setbit(m_txcon, T32, tf);
    //if turned on, bit will 'stick'
    //so must be T2, make sure T3 SIDL is off
    if(tf && r.anybit(m_txcon, T32)){
       r.clrbit(m_txcon+TMR23_SPACING, SIDL);
    }
}
void Timer23::clk_src(CLK e) const {
    r.setbit(m_txcon, TCS, e);
}
