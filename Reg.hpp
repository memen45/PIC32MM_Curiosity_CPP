#pragma once

/*=============================================================================
 Register (SFR) writes/read
 ksgex address <-> physical address
=============================================================================*/

#include <cstdint>

struct Reg {

    //public functions
    //set/clear a bit or bits to specified level (default=set=1)
    //(CLR register offset is +1, SET register offset is +2)
    //setb(address, bitmask)-> *(uint32_t*)address+CLR+1 = bitmask
    //setb(address, bitmask, 1)-> *(uint32_t*)address+CLR+1 = bitmask
    //setb(address, bitmask, 0)-> *(uint32_t*)address+CLR+0|1 = bitmask
    template <typename T> static void       setb    (T, uint32_t, bool = true);

    //flip bitmask bit(s) in register
    //(INV register offset is +3)
    //flipb(address, bitmask)
    template <typename T> static void       flipb   (T, uint32_t);

    //test if any bitmask bit(s) in the register are set/clear
    //anybit(address, bitmask)-> return *(uint32_t*)address & bitmask
    //anybit(address, bitmask, 1)-> return *(uint32_t*)address & bitmask
    //anybit(address, bitmask, 0)-> return !(*(uint32_t*)address & bitmask)
    template <typename T> static bool       anybit  (T, uint32_t, bool = true);

    //test if all bitmask bit(s) in the register are set/clear
    //allbits(address, bitmask)->
    //  return (*(uint32_t*)address & bitmask) == bitmask
    //allbits(address, bitmask)->
    //  return (*(uint32_t*)address & bitmask) == bitmask
    //allbits(address, bitmask)->
    //  return (*(uint32_t*)address | ~bitmask) == ~bitmask
    template <typename T> static bool       allbits (T, uint32_t, bool);

    //I don't know how to specify different return types when calling,
    //so am just creating 3 different versions- use for val wanted- will
    //also then use val* as T (can do byte read for 8bits, etc.)
    template <typename T> static uint32_t   val     (T);
    template <typename T> static uint16_t   val16   (T);
    template <typename T> static uint8_t    val8    (T);

    //make T a V*, so can use the byte or half-word address
    //(V type determines access)
    template <typename T, typename V> static void val(T, V);

    //physical to kseg0/1 addr, kseg to physical addr
    template <typename T> static uint32_t   p2kseg1 (T); //unused in PIC32MM
    template <typename T> static uint32_t   p2kseg0 (T);
    template <typename T> static uint32_t   k2phys  (T);

    private:

    //register word offset to atomic access registers
    enum { CLR = 1, SET, INV };

};

/*=============================================================================
 all functions inline
=============================================================================*/
//set/clr v bit(s) in register r (sc = 0/clr,1/set, default=1)
template <typename T> void Reg::setb(T r, uint32_t v, bool sc){
    *(reinterpret_cast <volatile uint32_t*>(r)+CLR+sc) = v;
}
//invert v bit(s) in register r
template <typename T> void Reg::flipb(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+INV) = v;
}
//check if any bit v (bitmask) in register r is set/clear (sc=1,0)
template <typename T> bool Reg::anybit(T r, uint32_t v, bool sc){
    if(sc) return (*(reinterpret_cast <volatile uint32_t*>(r)) & v);
    else return !(*(reinterpret_cast <volatile uint32_t*>(r)) & v);
}
//check if all bitmask v is set/clr (sc=1,0) in register r
template <typename T> bool Reg::allbits(T r, uint32_t v, bool sc){
    if(sc){
    return (*(reinterpret_cast <volatile uint32_t*>(r)) & v) == v;
    } else {
    return (*(reinterpret_cast <volatile uint32_t*>(r)) | ~v) == ~v;
    }
}
//return uint32_t value of register r
template <typename T> uint32_t Reg::val(T r){
    return *(reinterpret_cast <volatile uint32_t*>(r));
}
//return uint16_t value of register r
template <typename T> uint16_t Reg::val16(T r){
    return *(reinterpret_cast <volatile uint16_t*>(r));
}
//return uint16_t value of register r
template <typename T> uint8_t Reg::val8(T r){
    return *(reinterpret_cast <volatile uint8_t*>(r));
}
//set uint32_t value to register r
template <typename T, typename V> void Reg::val(T r, V v){
    *(reinterpret_cast <volatile V*>(r)) = v;
}


//physical to kseg0/1 addr, kseg to physical addr
//(kseg1 not used in PIC32MM- no cache)
template <typename T> uint32_t Reg::p2kseg1(T r){
    return (reinterpret_cast <uint32_t>(r)) | 0xA0000000; //A0=0b1010_0000
}
template <typename T> uint32_t Reg::p2kseg0(T r){
    return (reinterpret_cast <uint32_t>(r) | 0x80000000); //80=0b1000_0000
}
template <typename T> uint32_t Reg::k2phys(T r){
    return (reinterpret_cast <uint32_t>(r)) & 0x1FFFFFFF; //1F=0b0001_1111
}