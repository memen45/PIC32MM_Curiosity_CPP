#include "Spi.hpp"
#include "Osc.hpp"
#include "Reg.hpp"

enum :uint32_t {
    SPIX_SPACING = 0x200, //spacing in words
    SPIXCON_BASE = 0xBF805800,      //Address for SPI3, but the others follow in memory
        FRMEN = 31,
        FRMSYNC = 30,
        FRMPOL = 29,
        MSSEN = 28,
        FRMSYPW = 27,
        FRMCNT_SHIFT = 24, FRMCNT_MASK = 7,
        SPIFE = 17,
        ENHBUF = 16,
        ON = 15,
        MODE_SHIFT = 10, MODE_MASK = 3,
        SMP = 9,
        CKE = 8,
        SSEN = 7,
        CKP = 6,
        MSTEN = 5,
        STXISEL_SHIFT = 2, STXISEL_MASK = 3,
        SRXISEL_SHIFT = 0, SRXISEL_MASK = 3,
    SPIXSTAT = 4, //offset from SPIXCON in words
        SPIBUSY = 11,
        SPITUR = 8,
        SRMT = 7,
        SPIROV = 6,
        SPIRBE = 5,
        SPITBE = 3,
        SPITBF = 1,
        SPIRBF = 0,
    SPIXBUF = 8, //offset from SPIXCON in words
    SPIXBRG = 12, //offset from SPIXCON in words
};

//Spi

//=============================================================================
                Spi::Spi        (SPIX e)
//=============================================================================
    : m_spix_con((vu32ptr)SPIXCON_BASE + (e * SPIX_SPACING)),
      m_spixbuf(*((vu32ptr)SPIXCON_BASE + (e * SPIX_SPACING) + SPIXBUF)),
      m_spix_freq(0)
{
}

//spixcon
//=============================================================================
    void        Spi::frame              (bool tf)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<FRMEN, tf);
}

//=============================================================================
    void        Spi::frame_dir          (FRMDIR e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<FRMSYNC, e);
}

//=============================================================================
    void        Spi::frame_pol          (FRMHL e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<FRMPOL, e);
}

//=============================================================================
    void        Spi::slave_sel          (bool tf)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<MSSEN, tf);
}

//=============================================================================
    void        Spi::frame_pwidth       (FRMPW e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<FRMSYPW, e);
}

//=============================================================================
    void        Spi::frame_count        (FRMCNT e)
//=============================================================================
{
    Reg::clrbit(m_spix_con, FRMCNT_MASK<<FRMCNT_SHIFT);
    Reg::setbit(m_spix_con, e<<FRMCNT_SHIFT);
}

//=============================================================================
    void        Spi::frame_edge         (FRMEDGE e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<SPIFE, e);
}

//=============================================================================
    void        Spi::enhanced           (bool tf)
//=============================================================================
{
    bool ison = Reg::anybit(m_spix_con, 1<<ON);
    on(false);
    Reg::setbit(m_spix_con, 1<<ENHBUF, tf);
    on(ison);
}

//=============================================================================
    void        Spi::on                 (bool tf)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<ON, tf);
}

//=============================================================================
    void        Spi::mode               (MODE e)
//=============================================================================
{
    Reg::clrbit(m_spix_con, MODE_MASK<<MODE_SHIFT);
    Reg::setbit(m_spix_con, e<<MODE_SHIFT);
}

//=============================================================================
    void        Spi::phase              (PHASE e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<SMP, e);
}

//=============================================================================
    void        Spi::clk_edge           (CLKEDGE e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<CKE, e);
}

//=============================================================================
    void        Spi::ss                 (bool tf)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<SSEN, tf);
}

//=============================================================================
    void        Spi::clk_pol            (CLKPOL e)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<CKP, e);
}

//=============================================================================
    void        Spi::master             (bool tf)
//=============================================================================
{
    Reg::setbit(m_spix_con, 1<<MSTEN, tf);
}

//=============================================================================
    void        Spi::tx_irq             (TXIRQ e)
//=============================================================================
{
    Reg::clrbit(m_spix_con, STXISEL_MASK<<STXISEL_SHIFT);
    Reg::setbit(m_spix_con, e<<STXISEL_SHIFT);
}

//=============================================================================
    void        Spi::rx_irq             (RXIRQ e)
//=============================================================================
{
    Reg::clrbit(m_spix_con, SRXISEL_MASK<<SRXISEL_SHIFT);
    Reg::setbit(m_spix_con, e<<SRXISEL_SHIFT);
}

//spixstat
//=============================================================================
    uint8_t     Spi::stat_rxcount       ()
//=============================================================================
{
    return Reg::val8((vu8ptr)m_spix_con + (SPIXSTAT * 4) + 3);
}

//=============================================================================
    uint8_t     Spi::stat_txcount       ()
//=============================================================================
{
    return Reg::val8((vu8ptr)m_spix_con + (SPIXSTAT * 4) + 2);
}

//=============================================================================
    bool        Spi::stat_busy          ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPIBUSY);
}

//=============================================================================
    bool        Spi::stat_txurun        ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPITUR);
}

//=============================================================================
    bool        Spi::stat_srempty        ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SRMT);
}

//=============================================================================
    bool        Spi::stat_oerr          ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPIROV);
}

//=============================================================================
    void        Spi::stat_oerrclr       ()
//=============================================================================
{
    Reg::clrbit(m_spix_con + SPIXSTAT, 1<<SPIROV);
}

//=============================================================================
    bool        Spi::stat_rxempty        ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPIRBE);
}

//=============================================================================
    bool        Spi::stat_txempty        ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPITBE);
}

//=============================================================================
    bool        Spi::stat_txfull        ()
//=============================================================================
{
    return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPITBF);
}

//=============================================================================
    bool        Spi::stat_rxfull        ()
//=============================================================================
{
     return Reg::anybit(m_spix_con + SPIXSTAT, 1<<SPIRBF);
}

//spixbrg
//=============================================================================
    void        Spi::baud               (uint16_t v)
//=============================================================================
{
    Reg::val(m_spix_con + SPIXBRG, v);
}

//set frequency
//=============================================================================
    void        Spi::freq               (uint32_t v)
//=============================================================================
{
    uint32_t clk;
//   if(clk_sel() == REFO1) clk = Osc::refo_freq();
 //   else 
    clk = Osc::pbclk();
    uint16_t brg = (clk / (2 * v)) - 1;
    brg and_eq 0x1ff;
    baud(brg);
    freq();
}

//get actual frequency
//called by clk_sel(), freq(uint32_t)
//=============================================================================
    uint32_t    Spi::freq               ()
//=============================================================================
{
    uint32_t clk;
//    if(clk_sel() == REFO1) clk = Osc::refo_freq();
//    else 
    clk = Osc::pbclk();
    m_spix_freq = clk / (2 * (Reg::val16(m_spix_con + SPIXBRG) + 1));
    return m_spix_freq;
}

//spixbuf
//=============================================================================
    void            Spi::write      (uint32_t v)
//=============================================================================
{
    m_spixbuf = v; //m_spibuf&
}

//=============================================================================
    uint32_t        Spi::read       ()
//=============================================================================
{
    return m_spixbuf; //m_spibuf&
}
