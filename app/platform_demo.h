/**
 * @file platform_if.h
 *
 * This is the platform that shall to be implemented regardless
 * of the underlying
 *
 * */

#ifndef INC_PLATFORM_IF_H_
#define INC_PLATFORM_IF_H_

#include <lib/type.h>

typedef enum _exo_btn_action {
    EXO_BUTTON_PRESS = 0, EXO_BUTTON_RELEASE, EXO_BUTTON_CHANGE
} exo_btn_action;

/* General LED, Switch and Button identifiers */
#define EXO_LED_1          0
#define EXO_LED_2          1
#define EXO_LED_3          2
#define EXO_LED_4          3

#define EXO_SWITCH_1       0
#define EXO_SWITCH_2       1
#define EXO_SWITCH_3       2
#define EXO_SWITCH_4       3

#define EXO_BUTTON_1       0
#define EXO_BUTTON_2       1
#define EXO_BUTTON_3       2
#define EXO_BUTTON_4       3

#define EXO_THERMOMETER    0
#define EXO_POTMETER       1

struct sNetworkParams {
    char* SSID;
    char* PASSW;
};


/**
 * Initalize the HW platform
 * This can include
 *  - setting up timers/HW peripherals/CPU initalization
 *  - setting up drivers
 *  - setting up heap
 *  - setting up any resources that is needed on this platform
 *
 * @param: None
 * @return ERR_SUCCESS or a ERR_* failure value
 *
 **/
struct platform_init_class {
    void (*platform_periodic_fn)(void *data);
    int32_t platform_fn_period;
};

int32_t platform_init(struct platform_init_class *init);

typedef int (*platform_debug_callback)(const char *format, ... );
platform_debug_callback platform_get_debug_printf(void);
/**
 * Joint to a network
 *
 * @param: Network parameters
 * @return ERR_SUCCESS or a ERR_* failure value
 *
 **/
int32_t platform_join_network(struct sNetworkParams *params);

/*
 * Return 1 if the network infrastructure is ready
 * to work. For example the WiFi adapter has joined
 * to an access point
 *
 * @return 1 if the network is ready. Otherwise 0.
 * OS
 * */
uint8_t platform_is_network_ready(void);

/**
 * Kick the watchdog timer
 * The platform init should initalize the Watchdog if needed
 *
 **/
void platform_wdt_kick(void);

/**
 * Turn led on/off on the current board
 *
 * @param[in] on True for turning on/False for turning off the led
 *
 **/
int32_t platform_set_led(int32_t led, BOOL on);

/**
 * Get state of a switch on the current board
 *
 * @param[in]  sw      Identifier of the switch
 * @param[out] state   True if the switch is on
 *
 * @return ERR_SUCESS on success
 **/
int32_t platform_get_switch(int32_t sw, BOOL* state);

/**
 * Register for a button action
 *
 * @param[in]  btn    Identifier of the btton
 * @param[out] type   Type of the button action:
 *                       - EXO_BUTTON_PRESS
 *                       - EXO_BUTTON_RELEASE
 * @param[out] clb    Callback function
 *
 * @return ERR_SUCESS on success
 **/
int32_t platform_register_button_action(int32_t btn, exo_btn_action type,
        void (*clb)(int32_t));

/**
 * Get state of a switch on the current board
 *
 * @param[in]    type    Type of the sensor
 * @param[in]    id      The id of the sensor
 * @param[out]   buffer  The sensor's value in string format
 * @param[in]    size    The size of the output buffer
 *
 * @return ERR_SUCESS on success
 **/
int32_t platform_get_sensor_value(int32_t type, int32_t id, char *buffer, size_t len);
#endif /* INC_PLATFORM_IF_H_ */
