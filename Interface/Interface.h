/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_INTERFACE_H
#define FINEMOTE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
    void Setup();
    void Loop();
#ifdef __cplusplus
};
#endif

#define INRANGE(NUM, MIN, MAX) \
    NUM = ((NUM > (MIN) ? NUM : (MIN)) \
          < (MAX) ? NUM : (MAX))

#endif //FINEMOTE_INTERFACE_H
