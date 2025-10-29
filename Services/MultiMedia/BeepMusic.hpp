/*******************************************************************************
 * Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_BEEPMUSIC_HPP
#define FINEMOTE_BEEPMUSIC_HPP

#include "MultiMedia/MusicLibrary.hpp"
#include "DeviceBase.h"
#include "Bus/PWM_Base.hpp"

template <size_t ID>
class BeepMusic : public DeviceBase {
public:
    explicit BeepMusic(uint8_t songIndex = 0) {
        Play(0);
    }

    void Play(uint8_t songIndex) {
        activate = true;
        curSong = songIndex;
        curNote = MusicScript::Get()[curSong].script;
        BeepImpl(MusicScript::Get()[curSong].script->Note);
    }

    void Stop() {
        activate = false;
        BeepService();
    }

    void Handle() final {
        if (activate) {
            BeepService();
        }
    }

private:
    void BeepImpl(uint32_t freq) {
        if (0 == freq) {
            PWM_Base<ID>::GetInstance().SetDutyCycle(0);
        } else {
            PWM_Base<ID>::GetInstance().SetFrequency(freq);
            PWM_Base<ID>::GetInstance().SetDutyCycle(0.5);
        }
    }

    void BeepService() {

        static uint32_t noteTimer = 0;

        if (activate) {
            if (curNote && noteTimer >= curNote->Time) {
                noteTimer = 0;
                curNote++;
                if (curNote - const_cast<MusicNote_t *>(MusicScript::Get()[curSong].script) >= MusicScript::Get()[curSong].noteNum) {
                    Stop();
                } else {
                    BeepImpl(curNote->Note);
                }
            }
            noteTimer++;
        } else {
            noteTimer = 0;
            BeepImpl(0);
        }
    }

    uint8_t curSong;
    const MusicNote_t *curNote = nullptr;
    bool activate = false;
};

#endif
