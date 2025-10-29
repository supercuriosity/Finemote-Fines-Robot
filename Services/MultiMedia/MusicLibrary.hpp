/*******************************************************************************
* Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef MUSICLIBRARY_HPP
#define MUSICLIBRARY_HPP

#include "MultiMedia/Music_SuperMario.h"
#include "MultiMedia/Music_LiuSha.h"
#include "MultiMedia/Music_YMCA.h"
#include "MultiMedia/Music_Windmill.h"
#include "MultiMedia/Music_Astronomia.h"
#include "MultiMedia/Music_IronTorrent.h"
#include "MultiMedia/Music_Senbonzakura.h"

struct MusicScript {
    static auto& Get() {
        static const Song_t songs[] = {
            SONG_INIT(SuperMario),
            SONG_INIT(LiuSha),
            SONG_INIT(YMCA),

            SONG_INIT(Windmill),
            SONG_INIT(Astronomia),
            SONG_INIT(IronTorrent),
            SONG_INIT(Senbonzakura),
        };
        return songs;
    }
};

#endif
