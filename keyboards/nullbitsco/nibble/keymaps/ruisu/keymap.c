/* Copyright 2020 Jay Greco
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

enum layer_names {
  _MA,
  _FN
};

enum custom_keycodes {
    KC_CUST = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MA] = LAYOUT_ansi(
                KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_HOME,
        KC_F13, KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL,
        KC_F14, KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_PGUP,
        KC_F15, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          KC_UP,   KC_PGDN,
        KC_F16, KC_LCTL, KC_LGUI, KC_LALT,                   KC_SPC,                    MO(_FN), KC_RALT, KC_RCTL, KC_LEFT,          KC_DOWN, KC_RGHT
    ),
    [_FN] = LAYOUT_ansi(
                 RESET,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,  KC_END,
        RGB_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
        _______, _______, _______, _______,                   _______,                   _______, _______, _______, _______,          _______, _______
    ),
};

#ifdef OLED_DRIVER_ENABLE

typedef enum {
    OLED_MODE_IDLE = 0,
    OLED_MODE_VOLUME_UP = 1,
    OLED_MODE_VOLUME_DOWN = 2,
    OLED_MODE_RGB_ON = 3,
    OLED_MODE_RGB_OFF = 4,
} oled_mode_t;

static uint16_t oled_timer = 0;
oled_mode_t oled_mode = 0;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    oled_timer = timer_read32();
    return OLED_ROTATION_180;
}


static const char PROGMEM oled_mode_messages [5][15] = {
    "",
    "Volume Up",
    "Volume Down",
    "RGB ON",
    "RGB OFF"
};

static const char PROGMEM oled_mode_icons[][3][5] = {
    {{128,129,130,131,0},{160,161,162,163,0},{192,193,194,195,0}},
    {{132,133,134,135,0},{164,165,166,167,0},{196,197,198,199,0}},
    {{136,137,138,139,0},{168,169,170,171,0},{200,201,202,203,0}},
    {{140,141,142,143,0},{172,173,174,175,0},{204,205,206,207,0}},
    {{144,145,146,147,0},{176,177,178,179,0},{208,209,210,211,0}}
};



bool process_record_encoder_oled(uint16_t keycode) {
    oled_timer = timer_read32();
    switch (keycode) {
        case KC_VOLU:
            oled_mode = OLED_MODE_VOLUME_UP;
            break;
        case KC_VOLD:
            oled_mode = OLED_MODE_VOLUME_DOWN;
            break;
        default:
            oled_mode = OLED_MODE_IDLE;
            break;
    }
    return true;
}

bool process_record_keymap_oled(uint16_t keycode, keyrecord_t *record) {
    oled_timer = timer_read32();
    if(rgblight_is_enabled()) {
        oled_mode = OLED_MODE_RGB_OFF;
    } else {
        oled_mode = OLED_MODE_RGB_ON;
    }
    return true;
}

static void render_WPM(void) {
    char wpm_str[10];

    sprintf(wpm_str, "%03d", get_current_wpm());
    oled_write_P(PSTR("                 "), false);
    oled_write(wpm_str, false);
}

static void render_idle(void) {
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();

    oled_write_P(oled_mode_icons[0][0], false);
    oled_write_P(PSTR("        "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("S ") : PSTR("  "), false);
    oled_write_P(led_state.num_lock ? PSTR("N ") : PSTR("  "), false);
    oled_write_P(led_state.caps_lock ? PSTR("C ") : PSTR("  "), false);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(oled_mode_icons[0][1], false);
    oled_write_P(PSTR("  FuRiNaX"), false);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(oled_mode_icons[0][2], false);
    oled_write_P(PSTR("\n"), false);
    render_WPM();
}

static void render_status_mode_message(void) {
    oled_write_P(oled_mode_icons[oled_mode][0], false);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(oled_mode_icons[oled_mode][1], false);
    oled_write_P(PSTR("  "), false);
    oled_write_P(oled_mode_messages[oled_mode], false);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(oled_mode_icons[oled_mode][2], false);
    oled_write_P(PSTR("\n"), false);
}

static void render_frame(void) {
    switch (oled_mode) {
        case OLED_MODE_VOLUME_UP:
        case OLED_MODE_VOLUME_DOWN:
        case OLED_MODE_RGB_ON:
        case OLED_MODE_RGB_OFF:
            render_status_mode_message();
            break;
        default:
            render_idle();
            break;
    }
}

void oled_task_user(void) {
     if (timer_elapsed(oled_timer) >= 3000) {
        oled_mode = 0;
    }
    render_frame();
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Send keystrokes to host keyboard, if connected (see readme)
    process_record_remote_kb(keycode, record);

    switch(keycode) {
        case RGB_TOG:
            if (record->event.pressed) {
                process_record_keymap_oled(keycode, record);
            }
        break;
        case KC_CUST: //custom macro
            if (record->event.pressed) {
            }
        break;
    }
    return true;
}


void encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU);
        process_record_encoder_oled(KC_VOLU);
    } else {
        tap_code(KC_VOLD);
        process_record_encoder_oled(KC_VOLD);
    }
}

void matrix_init_user(void) {
    // Initialize remote keyboard, if connected (see readme)
    matrix_init_remote_kb();
}

void matrix_scan_user(void) {
    // Scan and parse keystrokes from remote keyboard, if connected (see readme)
    matrix_scan_remote_kb();
}
