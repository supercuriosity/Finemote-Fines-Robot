/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_MUSIC_SUPERMARIO_H
#define FINEMOTE_MUSIC_SUPERMARIO_H

#include "BeepMusicBasic.h"

#ifdef FINEMOTE_BEEPMUSIC_C

const MusicNote_t SuperMario_s[] = {
        {H3, 100}, {_0, 50},
        {H3, 250}, {_0, 50},
        {H3, 100}, {_0, 50},
        {_0, 150},
        {H1, 100}, {_0, 50},
        {H3, 250}, {_0, 50},
        {H5, 250}, {_0, 50},
        {_0, 300},
        {M5, 250}, {_0, 50},
        {_0, 300},
        {H1, 250}, {_0, 50}
};
#endif //FINEMOTE_BEEPMUSIC_C

#endif //FINEMOTE_MUSIC_SUPERMARIO_H
