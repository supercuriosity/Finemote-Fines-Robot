/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BEEPMUSIC_H
#define FINEMOTE_BEEPMUSIC_H

#include "BeepMusicBasic.h"

class BeepMusic {
public:
    explicit BeepMusic(void (*handle)(uint32_t freq));

    void Play(int16_t songIndex);
    void Stop();

    void BeepService();

    static BeepMusic MusicChannels[];
private:
    static Song_t songs[];
    void (*beepHandle)(uint32_t freq);

    int16_t curSong = -1;
    MusicNote_t* curNote = nullptr;
};

#endif //FINEMOTE_BEEPMUSIC_H
