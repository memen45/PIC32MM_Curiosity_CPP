#include "I2c.hpp"
#include "Osc.hpp"
#include "Reg.hpp"

enum :uint32_t {
    I2C1CON = 0xBF801500, I2CX_SPACING = 0x40, //spacing in words
        PCIE = 1<<22,
        SCIE = 1<<21,
        BOEN = 1<<20,
        SDAHT = 1<<19,
        SBCDE = 1<<18,
        ON = 1<<15,
        SIDL = 1<<13,
        SCLREL = 1<<12,
        STRICT = 1<<11,
        A10M = 1<<10,
        DISSLW = 1<<9,
        SMEN = 1<<8,
        GCEN = 1<<7,
        STREN = 1<<6,
        ACKDT = 1<<5,
        ACKEN = 1<<4,
        RCEN = 1<<3,
        PEN = 1<<2,
        RSEN = 1<<1,
        SEN = 1<<0,
    I2CXSTAT = 4, //offset from I2xCON in words
    I2CXADDR = 8,
    I2CXMSK = 12,
    I2CXBRG = 16,
    I2CXTRN = 20,
    I2CXRCV = 24
};

//=============================================================================
                I2c::I2c                (I2CX e)
//=============================================================================
    : m_i2cx_con((volatile uint32_t*)I2C1CON + (e * I2CX_SPACING)),
      m_speed(KHZ100)
{
}

//I2C1CON
//=============================================================================
    void        I2c::irq_stop           (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, PCIE, tf);
}

//=============================================================================
    void        I2c::irq_start          (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SCIE, tf);
}

//=============================================================================
    void        I2c::overwrite          (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, BOEN, tf);
}

//=============================================================================
    void        I2c::hold_time          (HOLDTIM e)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SDAHT, e);
}

//=============================================================================
    void        I2c::irq_collision      (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SBCDE, tf);
}

//=============================================================================
    void        I2c::on                 (bool tf)
//=============================================================================
{
    //always set brg in case clock changed
    //or speed not changed since init
    speed(m_speed);
    Reg::setbit(m_i2cx_con, ON, tf);
}

//=============================================================================
    void        I2c::stop_idle          (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SIDL, tf);
}

//=============================================================================
    void        I2c::clk_release        (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SCLREL, tf);
}

//=============================================================================
    void        I2c::strict             (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, STRICT, tf);
}

//=============================================================================
    void        I2c::addr_10bit         (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, A10M, tf);
}

//=============================================================================
    void        I2c::slew_rate          (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, DISSLW, not tf);
}

//=============================================================================
    void        I2c::smb_levels         (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SMEN, tf);
}

//=============================================================================
    void        I2c::irq_gencall        (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, GCEN, tf);
}

//=============================================================================
    void        I2c::clk_stretch        (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, STREN, tf);
}

//=============================================================================
    void        I2c::ack                (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, ACKDT, not tf); //0=ACK,1=NACK
    Reg::setbit(m_i2cx_con, ACKEN);
}

//=============================================================================
    void        I2c::rx                 (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, RCEN, tf);
}

//=============================================================================
    void        I2c::stop               (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, PEN, tf);
}

//=============================================================================
    void        I2c::repstart           (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, RSEN, tf);
}

//=============================================================================
    void        I2c::start              (bool tf)
//=============================================================================
{
    Reg::setbit(m_i2cx_con, SEN, tf);
}

//I2CXSTAT
//=============================================================================
    bool        I2c::stat               (STAT e)
//=============================================================================
{
    return Reg::anybit(m_i2cx_con + I2CXSTAT, e);
}

//=============================================================================
    void        I2c::buscol_clr         ()
//=============================================================================
{
    Reg::clrbit(m_i2cx_con + I2CXSTAT, BUSCOL);
}

//=============================================================================
    void        I2c::txcol_clr          ()
//=============================================================================
{
    Reg::clrbit(m_i2cx_con + I2CXSTAT, TXCOL);
}

    //=============================================================================
    void        I2c::rxoflow_clr        ()
//=============================================================================
{
    Reg::clrbit(m_i2cx_con + I2CXSTAT, RXOFLOW);
}

//I2CXADDR
//=============================================================================
    void        I2c::addr               (uint16_t v)
//=============================================================================
{
    Reg::val(m_i2cx_con + I2CXADDR, v);
}

//I2CXMSK
//=============================================================================
    void        I2c::addr_mask          (uint16_t v)
//=============================================================================
{
    Reg::val(m_i2cx_con + I2CXMSK, v);
}

//I2CXBRG
//=============================================================================
    void        I2c::speed              (I2CSPEED e)
//=============================================================================
{
    uint32_t sck = e * 2;
    uint32_t clk = Osc::sysclk()<<6;
    clk = clk / sck - clk / 9615384; //1/9615384=104ns=Tpgd
    brg((clk>>6) - 2);
    m_speed = e;
}

//=============================================================================
    void        I2c::brg                (uint16_t v)
//=============================================================================
{
    if(v < 2) v = 2; //0,1 not allowed
    Reg::val(m_i2cx_con + I2CXBRG, v);
}

//I2CXTRN
//=============================================================================
    void        I2c::write              (uint8_t v)
//=============================================================================
{
    Reg::val(m_i2cx_con + I2CXTRN, v);
}

//I2CXRCV
//=============================================================================
    uint8_t     I2c::read               ()
//=============================================================================
{
    return Reg::val8(m_i2cx_con + I2CXRCV);
}