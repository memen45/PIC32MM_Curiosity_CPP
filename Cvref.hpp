#pragma once

//Voltage Reference (CVref, DAC)

#include <cstdint>
#include "Reg.hpp"

struct Cvref : private Reg {

            static auto
on          (bool) -> void;

            static auto
out         (bool) -> void;

            enum
REFSEL      { NONE, CVREF, AVDD };

            static auto
refsel      (REFSEL) -> void;

            static auto
dacdat      (uint8_t) -> void;

};
