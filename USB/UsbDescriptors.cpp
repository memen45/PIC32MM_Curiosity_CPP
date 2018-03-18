#include "UsbDescriptors.hpp"
#include "UsbCh9.hpp"

//CDC Demo
const UsbCh9::DeviceDescriptor_t device = {
    18,             //length
    UsbCh9::DEVICE, //1
    0x0200,         //(bcd) usb 2.0
    2,              //class (CDC device)
    0,              //subclass
    0,              //protocol
    64,             //max ep0 packet size
    0x04D8,         //VID
    0x000A,         //PID
    0x0100,         //(bcd) release number
    1,              //string index- manufacturer
    2,              // -product
    0,              // -serial number
    1               //#of configurations
};



const uint8_t config1[] = {
    //let function fill in size (third,fourth byte, word size)
    9, UsbCh9::CONFIGURATION, 0, 0, 2, 1, 0, UsbCh9::SELFPOWER, 50,
    //interface0
    9, UsbCh9::INTERFACE, 0, 0, 0, 2, 2, 1, 0,
    //cdc descriptors
    5, 36, 0, 16, 1,        //cdc header
    4, 36, 2, 2,            //acm, the last 2 is support line requests only
    5, 36, 6, 0, 1,         //union comm id=0, data id=1
    5, 36, 1, 0, 1,         //call management
    //endpoint  //8,0 = 0x0008
    7, UsbCh9::ENDPOINT, UsbCh9::IN1, UsbCh9::INTERRUPT, 8, 0, 2,
    //interface1
    9, UsbCh9::INTERFACE, 1, 0, 2, 10, 0, 0, 0,
    //endpoint //64,0 = 0x0040
    7, UsbCh9::ENDPOINT, UsbCh9::OUT2, UsbCh9::BULK, 64, 0, 0,
    //endpoint
    7, UsbCh9::ENDPOINT, UsbCh9::IN2, UsbCh9::BULK, 64, 0, 0
};


//Language code string descriptor
const char* strings[] = {
    "\x09\x04",                     //language (low byte, high byte)
    "Microchip Techonology Inc.",   //manufacturer
    "CDC RS-232 Emulation Demo"     //product
};




//private

//always 18bytes
uint16_t get_device(uint8_t* buf, uint16_t sz){
    if(sz < device.bLength) return 0;
    const uint8_t* dev = (const uint8_t*)&device;
    uint16_t i = 0;
    for(; i < device.bLength; i++) buf[i] = *dev++;
    return i;
}

//could be >255 bytes
uint16_t get_config(uint8_t* buf, uint16_t sz, uint8_t idx){
    if(idx != 1 or sz < sizeof(config1)) return 0;
    uint8_t* cfg = (uint8_t*)config1;
    uint16_t i = 0;
    for(; i < sizeof(config1); i++) buf[i] = *cfg++;
    buf[2] = i; //fill in total config size
    buf[3] = i>>8; //upper byte
    return i;
}

//up to 255bytes
uint16_t get_string(uint8_t* buf, uint16_t sz, uint8_t idx){
    if(idx > sizeof(strings)) return 0;
    const char* str = strings[idx];
    buf[1] = UsbCh9::STRING; //type
    uint16_t i = 0;
    for(; *str && i < sz-1; i += 2){
        buf[i] = *str++;
        buf[i+1] = 0; //wide char
    }
    buf[0] = i; //fill in string length (max 255)
    //string > 255chars, or buffer too small, return 0, else string size
    return (*str || i > 255) ? 0 : i;
}

//public

//m_setup_pkt.wValue high=descriptor type, low=index
//device=1, config=2, string=3
uint16_t UsbDescriptors::get(uint16_t wValue, uint8_t* buf, uint16_t sz){
    uint8_t idx = wValue;
    uint8_t typ = wValue>>8;
    if(typ == 1){ return get_device(buf, sz); }
    if(typ == 2){ return get_config(buf, sz, idx); }
    if(typ == 3){ return get_string(buf, sz, idx); }
    return 0;
}