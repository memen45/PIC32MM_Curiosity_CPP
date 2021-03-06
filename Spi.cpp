#include "Spi.hpp"
#include "Osc.hpp"

enum :uint32_t {
    SPIX_SPACING = 0x80, //spacing in words
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
            Spi::
Spi         (SPIX e)
            : m_spix_con((vu32ptr)SPIXCON_BASE + (e * SPIX_SPACING)),
            m_spixbuf(*((vu32ptr)SPIXCON_BASE + (e * SPIX_SPACING) + SPIXBUF)),
            m_spix_freq(0)
            {   
            }

//spixcon
//=============================================================================
            auto Spi::
frame       (bool tf) -> void
            {
            setbit(m_spix_con, 1<<FRMEN, tf);
            }

//=============================================================================
            auto Spi::
frame_dir   (FRMDIR e) -> void
            {
            setbit(m_spix_con, 1<<FRMSYNC, e);
            }

//=============================================================================
            auto Spi::
frame_pol   (FRMHL e) -> void
            {
            setbit(m_spix_con, 1<<FRMPOL, e);
            }

//=============================================================================
            auto Spi::
slave_sel   (bool tf) -> void
            {
            setbit(m_spix_con, 1<<MSSEN, tf);
            }

//=============================================================================
            auto Spi::
frame_pwidth (FRMPW e) -> void
            {
            setbit(m_spix_con, 1<<FRMSYPW, e);
            }

//=============================================================================
            auto Spi::
frame_count (FRMCNT e) -> void
            {
            clrbit(m_spix_con, FRMCNT_MASK<<FRMCNT_SHIFT);
            setbit(m_spix_con, e<<FRMCNT_SHIFT);
            }

//=============================================================================
            auto Spi::
frame_edge  (FRMEDGE e) -> void
            {
            setbit(m_spix_con, 1<<SPIFE, e);
            }

//=============================================================================
            auto Spi::
enhanced    (bool tf) -> void
            {
            bool ison = anybit(m_spix_con, 1<<ON);
            on(false);
            setbit(m_spix_con, 1<<ENHBUF, tf);
            on(ison);
            }

//=============================================================================
            auto Spi::
on          (bool tf) -> void
            {
            setbit(m_spix_con, 1<<ON, tf);
            }

//=============================================================================
            auto Spi::
mode        (MODE e) -> void
            {
            clrbit(m_spix_con, MODE_MASK<<MODE_SHIFT);
            setbit(m_spix_con, e<<MODE_SHIFT);
            }

//=============================================================================
            auto Spi::
phase       (PHASE e) -> void
            {
            setbit(m_spix_con, 1<<SMP, e);
            }

//=============================================================================
            auto Spi::
clk_edge    (CLKEDGE e) -> void
            {
            setbit(m_spix_con, 1<<CKE, e);
            }

//=============================================================================
            auto Spi::
ss          (bool tf) -> void
            {
            setbit(m_spix_con, 1<<SSEN, tf);
            }

//=============================================================================
            auto Spi::
clk_pol     (CLKPOL e) -> void
            {
            setbit(m_spix_con, 1<<CKP, e);
            }

//=============================================================================
            auto Spi::
master      (bool tf) -> void
            {
            setbit(m_spix_con, 1<<MSTEN, tf);
            }

//=============================================================================
            auto Spi::
tx_irq      (TXIRQ e) -> void
            {
            clrbit(m_spix_con, STXISEL_MASK<<STXISEL_SHIFT);
            setbit(m_spix_con, e<<STXISEL_SHIFT);
            }

//=============================================================================
            auto Spi::
rx_irq      (RXIRQ e) -> void
            {
            clrbit(m_spix_con, SRXISEL_MASK<<SRXISEL_SHIFT);
            setbit(m_spix_con, e<<SRXISEL_SHIFT);
            }

//spixstat
//=============================================================================
            auto Spi::
stat_rxcount () -> uint8_t
            {
            return val8((vu8ptr)m_spix_con + (SPIXSTAT * 4) + 3);
            }

//=============================================================================
            auto Spi::
stat_txcount () -> uint8_t
            {
            return val8((vu8ptr)m_spix_con + (SPIXSTAT * 4) + 2);
            }

//=============================================================================
            auto Spi::
stat_busy   () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPIBUSY);
            }

//=============================================================================
            auto Spi::
stat_txurun () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPITUR);
            }

//=============================================================================
            auto Spi::
stat_sremty () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SRMT);
            }

//=============================================================================
            auto Spi::
stat_oerr   () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPIROV);
            }

//=============================================================================
            auto Spi::
stat_oerrclr () -> void
            {
            clrbit(m_spix_con + SPIXSTAT, 1<<SPIROV);
            }

//=============================================================================
            auto Spi::
stat_rxemty () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPIRBE);
            }

//=============================================================================
            auto Spi::
stat_txemty () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPITBE);
            }

//=============================================================================
            auto Spi::
stat_txfull () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPITBF);
            }

//=============================================================================
            auto Spi::
stat_rxfull () -> bool
            {
            return anybit(m_spix_con + SPIXSTAT, 1<<SPIRBF);
            }

//spixbrg
//=============================================================================
            auto Spi::
baud        (uint16_t v) -> void
            {
            val(m_spix_con + SPIXBRG, v);
            }

//set frequency
//=============================================================================
            auto Spi::
freq        (uint32_t v) -> void
            {
            uint32_t clk;
            clk = Osc::pbclk();
            uint16_t brg = (clk / (2 * v)) - 1;
            brg and_eq 0x1ff;
            baud(brg);
            freq();
            }

//get actual frequency
//called by clk_sel(), freq(uint32_t)
//=============================================================================
            auto Spi::
freq        () -> uint32_t
            {
            uint32_t clk;
            clk = Osc::pbclk();
            m_spix_freq = clk / (2 * (val16(m_spix_con + SPIXBRG) + 1));
            return m_spix_freq;
            }

//spixbuf
//=============================================================================
            auto Spi::
write       (uint32_t v) -> void
            {
            m_spixbuf = v; //m_spibuf&
            }

//=============================================================================
            auto Spi::
read        () -> uint32_t
            {
            return m_spixbuf; //m_spibuf&
            }
