/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#define FINEMOTE_BEEPMUSIC_C

#include "BeepMusic.h"
#include "PeripheralDefination.h"

/*****   曲库   *****/
#include "Music_Astronomia.h"
#include "Music_IronTorrent.h"
#include "Music_Senbonzakura.h"
#include "Music_SuperMario.h"
#include "Music_Windmill.h"

Song_t BeepMusic::songs[] = {
    SONG_INIT(Astronomia),
    SONG_INIT(IronTorrent),
    SONG_INIT(Senbonzakura),
    SONG_INIT(SuperMario),
    SONG_INIT(Windmill),
};

BeepMusic::BeepMusic(void (*handle)(uint32_t freq)) {
    beepHandle = handle;
}

void BeepMusic::Play(int16_t songIndex) {
    curSong = songIndex;
    curNote = const_cast<MusicNote_t*>(songs[curSong].script);
    beepHandle(songs[curSong].script->Note);
}

void BeepMusic::Stop() {
    curSong = -1;
    beepHandle(0);
}

void BeepMusic::BeepService() {
    static uint32_t time = 0;
    if (curSong != -1) {
        if (time >= curNote->Time) {
            time = 0;
            curNote++;
            if(curNote - const_cast<MusicNote_t*>(songs[curSong].script) >= songs[curSong].noteNum) {
                Stop();
            } else {
                beepHandle(curNote->Note);
            }
        }

        time++;
    }
}

BeepMusic BeepMusic::MusicChannels[] = {
    BeepMusic([](uint32_t freq)->void {
        if(0 == freq){
            __HAL_TIM_SET_COMPARE(&TIM_Buzzer, TIM_Buzzer_Channel, 0);
        } else {
            __HAL_TIM_SET_AUTORELOAD(&TIM_Buzzer, 1000000 / freq);
            __HAL_TIM_SET_COMPARE(&TIM_Buzzer, TIM_Buzzer_Channel, 500000 / freq);
        }
    }),
};
