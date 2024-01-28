
/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_LED_H
#define FINEMOTE_LED_H
#include "ProjectConfig.h"
#ifdef LED_MODULE
class LED {
public:
    static void Toggle(){
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }

};
#endif

#endif //FINEMOTE_LED_H
