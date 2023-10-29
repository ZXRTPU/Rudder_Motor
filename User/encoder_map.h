#ifndef ENCODER_MAP_H
#define ENCODER_MAP_H

#include "main.h"
#include "rc_potocal.h"
#include "math.h"

//对当前角度即rotor_angle整定到标准编码盘上
uint16_t encoder_map_8191(uint16_t ZERO_POS,uint16_t rotor_angle);

#endif