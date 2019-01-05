
#ifndef MUSIC_JOY_H__
#define MUSIC_JOY_H__
#include "stdint.h"
#include "music_scale.h"

uint32_t const joyData[]=
{
  M3, M3, M4, M5,
  M5, M4, M3, M2,
  M1, M1, M2, M3,
  M3, M2, M2,
  M3, M3, M4, M5,
  M5, M4, M3, M2,
  M1, M1, M2, M3,
  M2, M1, M1,
  M2, M2, M3, M1,
  M2, M3, M4, M3, M1,
  M2, M3, M4, M3, M2,
  M1, M2, D5, M3,
  M3, M3, M4, M5,
  M5, M4, M3, M4, M2,
  M1, M1, M2, M3,
  M2, M1, M1
};
float const joyDurations[] = {
  1,    1,    1,    1,
  1,    1,    1,    1,
  1,    1,    1,    1,
  1.5,  0.5,  2,
  1,    1,    1,    1,
  1,    1,    1,    1,
  1,    1,    1,    1,
  1.5,  0.5,  2,
  1,    1,    1,    1,
  1,    0.5,  0.5,  1,    1,
  1,    0.5,  0.5,  1,    1,
  1,    1,    1,    1,
  1,    1,    1,    1,
  1,    1,    1,    0.5,  0.5,
  1,    1,    1,    1,
  1.5,  0.5,  2

};

uint32_t const liangliangData[]=
{
  M1, M1, M1, D7, M1, D7, M1,
  M1, M1, M1, D7, M1, M2, D7,
  D7, D7, D7, D6, D7, D6, D7,
  D7, D7, D7, D6, D7, D6, D5,
  D6,                 D0, D0,

  D0, D0, D6, D5, D3,
  D5, D2, D3, D3, D0,
  D0, D0, D0, D0,
  M1, M1, M1, D7, M1, D7, M1,
  M1, M1, M1, D7, M1, M2, D7,

  D7, D7, D7, D6, D7, D6, D7,
  D7, D7, D7, D6, D7, M1, D7, D5,
  D6, D0, D0,
  D0, D0, D6, M3, M2,
  M2, M3, M3,
  D0, D0, D0, D0,

  D6, M3, M3, M2, M3, M5, M3, M2,
  M3, D6, D0,
  D7, D7, D7, M1, D7, D3, D5,
  D6, D5, D5, D0,
  D6, M3, M3, M2, M3, M2, M3, M5,

  M5, M3, D0, M2, M3, M5,
  M5, M6, M6, M5,
  M5, M6, M6, D0,
  D6, M3, M3, M2, M3, M5, M3, M2,
  M3, D6, D0,
  D7, D7, D7, M1, D7, D3, D5,

  D6, D5, D5, D0,
  D6, M3, M3, M2, M3, M2, M3, M5,
  H1, M7, M6, M5, M3, M5,
  M6, M3, M5, M3, M5, M6,
  M6, D0, D0,
  M6, M5, M3, M5,
  M5, H1, M7, M6, M5,
  M6, D0,

};

float const liangliangDurations[] = {
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  2,    1,    1,

  1,    1,    0.5,  1,    0.5,
  0.25, 0.25, 0.5,  2,    1,
  1,    1,    1,    1,
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,

  0.5,  0.5,  0.5,  0.5,  1,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
  2,    1,    1,
  1,    1,    1,    0.5,  0.5,
  0.5,  0.5,  3,
  1,    1,    1,    1,

  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
  1,    2,  1,
  0.5,  0.5,  0.5,  0.5,  0.5,  1,    0.5,
  0.5,  0.5,  2,  1,
  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,

  0.5,  1.5,  0.5,  0.5,  0.5,  0.5,
  0.5,  1.5,  1.5,  0.5,
  0.5,  0.5,  2,  1,
  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
  1,    2,  1,
  0.5,  0.5,  0.5,  0.5,  0.5,  1,  0.5,

  0.5,  0.5,  2,  1,
  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,  0.5,
  0.5,  0.5,  0.5,  0.5,  1,    1,
  1,    1,    0.5,  0.5,  0.5,  0.5,
  2,    1,    1,
  1.5,  0.5,  0.5,  1.5,

  2,    0.5,  0.5,  0.5,  0.5,
  3,    1,
};

uint32_t const littleStarsData[]={
  M1, M1, M5, M5,
  M6, M6, M5,
  M4, M4, M3, M3,
  M2, M2, M1,
  M5, M5, M4, M4,
  M3, M3, M2,
  M5, M5, M4, M4,
  M3, M3, M2,
  M1, M1, M5, M5,
  M6, M6, M5,
  M4, M4, M3, M3,
  M2, M2, M1

};

float const littleStarsDurations[] =
{
  1,1,1,1,
  1,1,2,
  1,1,1,1,
  1,1,2,
  1,1,1,1,
  1,1,2,
  1,1,1,1,
  1,1,2,
  1,1,1,1,
  1,1,2,
  1,1,1,1,
  1,1,2
};


#endif

