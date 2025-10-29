// Copyright (c) 2025.
// IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
// All rights reserved.

#include "Scheduler.h"

#include "MultiMedia/BeepMusic.hpp"

enum class button_state_e {
    UNPRESSED = 0 ,
    PRESSED
};

BeepMusic<BUZZER_PWM_ID> MusicBuzzer(0);

constexpr unsigned int debounceChecks = 5;
constexpr auto buttonPressedLevel = GPIO_PIN_RESET;

static button_state_e buttonStableState = button_state_e::UNPRESSED;
static button_state_e buttonLastState = button_state_e::UNPRESSED;

static int songIndex = 0;

void updateButtonStateBitwise(button_state_e* current_state, bool rawIsPressed) {
    constexpr uint32_t debounceMask = (1U << debounceChecks) - 1;
    static uint32_t history = debounceMask;
    history = (history << 1) | rawIsPressed;
    if ((history & debounceMask) == 0) {
        *current_state = button_state_e::PRESSED;
    } else if ((history & debounceMask) == debounceMask) {
        *current_state = button_state_e::UNPRESSED;
    }
}

void TaskBeepMusic() {
    bool rawIsPressed = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == buttonPressedLevel);
    updateButtonStateBitwise(&buttonStableState,rawIsPressed );

    if (buttonStableState != buttonLastState) {
        if (buttonStableState == button_state_e::PRESSED) {
            songIndex = (songIndex + 1) % 5;
            MusicBuzzer.Play(songIndex);
        }
    }
    buttonLastState = buttonStableState;
}
TASK_EXPORT(TaskBeepMusic);

