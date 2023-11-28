/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BEEPMUSICBASIC_H
#define FINEMOTE_BEEPMUSICBASIC_H

#include <cstdint>
#include <string>

/**
 * 音高定义
 */
typedef enum : uint16_t {
    _0 = 0,

    LL5 = 196,
    LL6 = 220,

    L1 = 262,
    L1U = 277,
    L2 = 294,
    L2U = 311,
    L3 = 330,
    L4 = 349,
    L4U = 370,
    L5 = 392,
    L5U = 415,
    L6 = 440,
    L6U = 466,
    L7 = 494,

    M1 = 523,
    M1U = 554,
    M2 = 587,
    M2U = 622,
    M3 = 659,
    M4 = 698,
    M4U = 740,
    M5 = 784,
    M5U = 831,
    M6 = 880,
    M6U = 932,
    M7 = 988,

    H1 = 1046,
    H1U = 1109,
    H2 = 1175,
    H2U = 1245,
    H3 = 1318,
    H4 = 1397,
    H4U = 1480,
    H5 = 1568,
    H5U = 1661,
    H6 = 1760,
    H6U = 1865,
    H7 = 1976,
} Note_e;

typedef struct __attribute__((packed)){
    Note_e Note;
    uint16_t Time;
} MusicNote_t;

typedef struct {
    const std::string name;
    const MusicNote_t* const script;
    const uint16_t noteNum;
} Song_t;

#define SONG_INIT(_NAME) {#_NAME, _NAME##_s, sizeof(_NAME##_s)/sizeof(MusicNote_t)}

#endif //FINEMOTE_BEEPMUSICBASIC_H
