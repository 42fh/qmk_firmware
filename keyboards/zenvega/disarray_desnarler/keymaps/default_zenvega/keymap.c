// Copyright 2026 42fh
// SPDX-License-Identifier: GPL-2.0-or-later

#include "analog.h"
#include "gpio.h"
#include QMK_KEYBOARD_H

#define SLIDER_PIN GP29
#define LED1_PIN GP26
#define LED2_PIN GP28
#define LED3_PIN GP27
#define LAYER_SWITCH_PIN GP0

enum layers {
    _WORKSPACE,
    _MOVE_WORKSPACE,
    _APP_SWITCHER,
    _SYSTEM,
    _WINDOW,
    _RESIZE_WINDOW,
    _WINDOW_WORKSPACE,
    _WINDOW_SYSTEM
};

static uint32_t last_vol_change = 0;
static uint32_t last_tab_time   = 0;
static bool     gui_held        = false;
static bool     slider_ready    = false;
static bool     switch_on       = false;

static const int16_t slider_center    = 512;
static const int16_t slider_dead_zone = 70;

#define GUI_HOLD_TIMEOUT 1000
#define SLIDER_INTERVAL 100

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_WORKSPACE] = LAYOUT(
        MO(_MOVE_WORKSPACE), MO(_APP_SWITCHER),
        LGUI(LALT(KC_LEFT)), LGUI(LALT(KC_RIGHT))
    ),
    [_MOVE_WORKSPACE] = LAYOUT(
        _______, MO(_APP_SWITCHER),
        LGUI(LSFT(LALT(KC_LEFT))), LGUI(LSFT(LALT(KC_RIGHT)))
    ),
    [_APP_SWITCHER] = LAYOUT(
        MO(_MOVE_WORKSPACE), _______,
        LGUI(KC_TAB), LGUI(LSFT(KC_TAB))
    ),
    [_SYSTEM] = LAYOUT(
        _______, _______,
        LGUI(KC_L), KC_SYSTEM_SLEEP
    ),
    [_WINDOW] = LAYOUT(
        MO(_RESIZE_WINDOW), MO(_WINDOW_WORKSPACE),
        LGUI(KC_LEFT), LGUI(KC_RIGHT)
    ),
    [_RESIZE_WINDOW] = LAYOUT(
        _______, MO(_WINDOW_WORKSPACE),
        LGUI(KC_UP), LGUI(KC_DOWN)
    ),
    [_WINDOW_WORKSPACE] = LAYOUT(
        MO(_RESIZE_WINDOW), _______,
        LGUI(LALT(KC_LEFT)), LGUI(LALT(KC_RIGHT))
    ),
    [_WINDOW_SYSTEM] = LAYOUT(
        _______, _______,
        LGUI(KC_L), KC_SYSTEM_SLEEP
    )
};

static void set_layer_leds(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    gpio_write_pin_low(LED1_PIN);
    gpio_write_pin_low(LED2_PIN);
    gpio_write_pin_low(LED3_PIN);

    switch (layer) {
        case _WORKSPACE:
        case _WINDOW:
            gpio_write_pin_high(LED1_PIN);
            break;
        case _MOVE_WORKSPACE:
        case _RESIZE_WINDOW:
            gpio_write_pin_high(LED2_PIN);
            break;
        case _APP_SWITCHER:
        case _WINDOW_WORKSPACE:
            gpio_write_pin_high(LED3_PIN);
            break;
        case _SYSTEM:
        case _WINDOW_SYSTEM:
            gpio_write_pin_high(LED1_PIN);
            gpio_write_pin_high(LED2_PIN);
            gpio_write_pin_high(LED3_PIN);
            break;
    }
}

void matrix_init_user(void) {
    gpio_set_pin_input_high(LAYER_SWITCH_PIN);
    gpio_set_pin_output(LED1_PIN);
    gpio_set_pin_output(LED2_PIN);
    gpio_set_pin_output(LED3_PIN);

    set_layer_leds(layer_state);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, _MOVE_WORKSPACE, _APP_SWITCHER, _SYSTEM);
    state = update_tri_layer_state(state, _RESIZE_WINDOW, _WINDOW_WORKSPACE, _WINDOW_SYSTEM);
    set_layer_leds(state);
    return state;
}

void matrix_scan_user(void) {
    bool new_mode = !gpio_read_pin(LAYER_SWITCH_PIN);
    if (new_mode != switch_on) {
        switch_on = new_mode;
        layer_move(switch_on ? _WINDOW : _WORKSPACE);
    }

    if (gui_held && timer_elapsed32(last_tab_time) > GUI_HOLD_TIMEOUT) {
        unregister_mods(MOD_BIT(KC_LGUI));
        gui_held = false;
    }

    if (!slider_ready) {
        last_vol_change = timer_read32();
        slider_ready    = true;
        return;
    }

    if (timer_elapsed32(last_vol_change) < SLIDER_INTERVAL) {
        return;
    }

    int16_t raw = analogReadPin(SLIDER_PIN);
    if (raw < slider_center - slider_dead_zone) {
        tap_code(KC_AUDIO_VOL_DOWN);
        last_vol_change = timer_read32();
    } else if (raw > slider_center + slider_dead_zone) {
        tap_code(KC_AUDIO_VOL_UP);
        last_vol_change = timer_read32();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case LGUI(KC_TAB):
        case LGUI(LSFT(KC_TAB)):
            if (!gui_held) {
                register_mods(MOD_BIT(KC_LGUI));
                gui_held = true;
            }

            if (keycode == LGUI(KC_TAB)) {
                tap_code(KC_TAB);
            } else {
                register_mods(MOD_BIT(KC_LSFT));
                tap_code(KC_TAB);
                unregister_mods(MOD_BIT(KC_LSFT));
            }

            last_tab_time = timer_read32();
            return false;
    }

    return true;
}
