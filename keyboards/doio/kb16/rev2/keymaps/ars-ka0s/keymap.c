/* Copyright 2022 DOIO
 * Copyright 2022 HorrorTroll <https://github.com/HorrorTroll>
 * Copyright 2023 Matthew R. McDougal <https://github.com/ars-ka0s>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

extern MidiDevice midi_device;
#define MIDI_CC_OFF 0
#define MIDI_CC_ON 127
#define MIDI_CC_MID 63
#define MIDI_CC_DOWN 21
#define MIDI_CC_UP 105

// OLED animation
#include "./lib/layer_status/layer_status.h"

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.


enum layer_names {
    _BASE,
};

enum layer_keycodes {
    TCI_KC_0 = SAFE_RANGE,
    TCI_KC_1,
    TCI_KC_2,
    TCI_KC_3,
    TCI_KC_4,
    TCI_KC_5,
    TCI_KC_6,
    TCI_KC_7,
    TCI_KC_8,
    TCI_KC_9,
    TCI_KC_10,
    TCI_KC_11,
    TCI_KC_12,
    TCI_KC_13,
    TCI_KC_14,
    TCI_KC_15,

    TCI_E0_C,
    TCI_E1_C,
    TCI_E2_C,

    TCI_E0_U,
    TCI_E1_U,
    TCI_E2_U,

    TCI_E0_D,
    TCI_E1_D,
    TCI_E2_D,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT(
    TCI_KC_0, TCI_KC_1, TCI_KC_2, TCI_KC_3, TCI_E1_C,
    TCI_KC_4, TCI_KC_5, TCI_KC_6, TCI_KC_7, TCI_E2_C,
    TCI_KC_8, TCI_KC_9, TCI_KC_10, TCI_KC_11, TCI_E0_C,
    TCI_KC_12, TCI_KC_13, TCI_KC_14, TCI_KC_15
    )
};

#ifdef OLED_ENABLE
    bool oled_task_user(void) {
        render_layer_status();

        return true;
    }
#endif

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_BASE] = { ENCODER_CCW_CW(TCI_E1_D, TCI_E1_U), ENCODER_CCW_CW(TCI_E2_D, TCI_E2_U), ENCODER_CCW_CW(TCI_E0_D, TCI_E0_U) }
};
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode < TCI_KC_0) return false;
    if (keycode > TCI_E2_D) return false;

    if (keycode < TCI_E0_C) { // Map buttons to CC 102-117
        if (record->event.pressed) {
            midi_send_cc(&midi_device, midi_config.channel, 102 + (keycode - TCI_KC_0), MIDI_CC_ON);
        } else {
            midi_send_cc(&midi_device, midi_config.channel, 102 + (keycode - TCI_KC_0), MIDI_CC_OFF);
        }

        return true;
    }

    if (record->event.pressed) {
        uint8_t idx;
        int16_t cv;
        if (keycode < TCI_E0_U) {
            idx = keycode - TCI_E0_C;
            cv = MIDI_CC_MID;
        } else if (keycode < TCI_E0_D) {
            idx = keycode - TCI_E0_U;
            cv = MIDI_CC_UP;
        } else {
            idx = keycode - TCI_E0_D;
            cv = MIDI_CC_DOWN;
        }

        // Map knobs to CC 20-22
        midi_send_cc(&midi_device, midi_config.channel, 20 + idx, cv);
    }

    return true;
}
