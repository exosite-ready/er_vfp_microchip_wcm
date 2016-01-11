/** DESCRIPTION
 * -------------------
 *  This a Demo Application for the Microchip WCM boarad DM182020WCM
 *
 *  1.) It Checks the state of 4 buttons on the WCM Board (S1..S4) and
 *  sends a notification to the server if a button is pressed.
 *  The notification value is 1 for the 1st button press, 0 for the
 *  next button press, then 0 and so on.
 *
 *  2.) Parallel, it subscribes to the "led" data resource and toggles
 *     four LEDs (D1 .. D4) if it is set on the server side.
 *     The value of "led" contains the (desired) binary encoded states of the
 *     four LEDs, e.g.:
 *       value = 1 turns on 1st led
 *       value = 2 turns on 2nd led
 *       value = 3 turns on 1st and 2nd led
 *       value = 4 turns on 3rd led
 *       value = 5 turns off 1st and 3rd led
 *       value = 6 turns off 2nd and 3rd led
 *       value = 7 turns off 1st, 2nd and 3rd led
 *       value = 8 turns off 4th led
 *
 *   3.) It sends the value of the potentiometer every second
 *
 *  NOTE: If your device has not been activated, the activation process
 *  will be performed when this example runs first time. The given CIK will
 *  be stored in the device's permanent memory. Later this stored CIK
 *  will be used for the communication.
 *
 *  NOTE: Make sure that your device does not store an invalid CIK in
 *  its permanent storage. (For example the device was activated before,
 *  but the assigned device on portal has been deleted).
 *  If it does, you have to clear it to enforce a new activation process.
 *
 */

#include <stdio.h>
#include <lib/type.h>
#include <lib/error.h>
#include <lib/debug.h>
#include <porting/system_port.h>
#include <exosite_api.h>
#include <platform_demo.h>
#include <configurator_api.h>

/**
 * Sets the four leds based on a string value
 * The string value contains a number, which binary encodes 4 LED states
**/
static void set_leds(char *value)
{
    char *endptr;
    uint32_t led_value;

    led_value = strtol(value, &endptr, 0);
    if (endptr == value || *endptr != '\0')
        return;

    /* LED value demultiplexing
     * Values
     *   0th bit is led 0
     *   1st bit is led 1
     *   2nd bit is led 2
     *   3rdcbit is led 3
     */
    platform_set_led(0, led_value & 0x1);
    platform_set_led(1, (led_value >> 1) & 0x1);
    platform_set_led(2, (led_value >> 2) & 0x1);
    platform_set_led(3, (led_value >> 3) & 0x1);
}

/**
 *  This function is called when the read operation is finished.
 *
 *  @param[in] status   Status of the read operation. It is ERR_SUCCESS
 *                      in case of success. Otherwise it is <0.
 *  @param[in] alias    Alias of the data resource.
 *  @param[in] value    Value of the data resource
 */
static void on_read(int status, const char *alias, const char *value)
{
    if (status == ERR_SUCCESS) {
        info_log(DEBUG_APPLICATION, ("Read \"%s\": %s\n",alias, value));
        set_leds(value);
    }
}

/**
 *  This function is called when the write operation is finished.
 *
 *  @param[in] status   Status of the wrire operation. It is ERR_SUCCESS
 *                      in case of success. Otherwise it is <0.
 *  @param[in] alias    Alias of the data resource.
 *  @param[in] value    Value of the data resource
 */
static void on_write(int status, const char *alias)
{
    if (status == ERR_SUCCESS)
        info_log(DEBUG_APPLICATION,("Write \"%s\"\n",alias));
}

/**
 *  This function is called when the data resource changed on the server.
 *
 *  @param[in] status   Status of the read operation. It is ERR_SUCCESS
 *                      in case of success. Otherwise it is <0.
 *  @param[in] alias    Alias of the data resource.
 *  @param[in] value    Value of the data resource
 */
static void on_change(int status, const char *alias, const char *value)
{
    if (status == ERR_SUCCESS) {
        info_log(DEBUG_APPLICATION,("Value changed on server \"%s\" to %s \n",alias, value));
        set_leds(value);
    }
}

#define NUMBER_OF_BUTTONS 4
/**
 * This function looks for a 0 - 1 transition in the state of button[i]
 * Button id 0 - 3 identifies the buttons S1..S4
 * If it founds a 0 - 1 transition it return TRUE otherwise it returns FALSE
 *
 **/
static BOOL detect_edge(int button)
{
    static BOOL button_state[NUMBER_OF_BUTTONS];
    static BOOL previous_button_state[NUMBER_OF_BUTTONS] = {FALSE, FALSE, FALSE, FALSE};
    static BOOL leading_edge[NUMBER_OF_BUTTONS] = {FALSE, FALSE, FALSE, FALSE};
  
    platform_get_switch(button, &button_state[button]);
    if (previous_button_state[button] == FALSE && button_state[button] == TRUE)
        leading_edge[button] = TRUE;
    else
        leading_edge[button] = FALSE;

    if (button_state[button] != previous_button_state[button])
      previous_button_state[button] = button_state[button];

    if (leading_edge[button] == TRUE)
        return TRUE;
    
    return FALSE;
}
/**
 * Entry point of the demo
 */

#define MAX_VALUE_SIZE 16

volatile BOOL wifi_parameters_set;

static int32_t on_new_wifi_config(struct wifi_settings *wifi_cfg)
{
    struct sNetworkParams params;
    int32_t error;

    info_log(DEBUG_APPLICATION,
        ("New WiFi Config (SSID: %s; Security: %s; Pass: %s)\n",
            wifi_cfg->ssid,
            (wifi_cfg->security == WPA2) ? "WPA2" : (wifi_cfg->security == OPEN) ? "OPEN" : "Unknown",
            wifi_cfg->passpharase));

    params.SSID = wifi_cfg->ssid;
    params.PASSW = wifi_cfg->passpharase;
    error = platform_save_network_configuration(&params);
    wifi_parameters_set = TRUE;

    return error;
}

int main()
{
    struct exosite_class *exo = NULL;
    int error;
    int i;
    BOOL wifi_configured;
    struct sNetworkParams params;
    char * switch_alias[] = {"sw1", "sw2", "sw3", "sw4"};
    char potmeter_value_str[MAX_VALUE_SIZE];
    sys_time_t start_time;
    struct platform_init_class pl_init;
    enum exosite_device_status status;

    /** 1. Initialize the platform and configure and join the network */
    platform_init(&pl_init);
    /* Platform specific wifi configuration
     * PLATFORM_SPECIFIC_WIFI_CONFIG has to be set
     * This demo will use the wifi config method from
     * WCM DK
     * */
    platform_set_led(0, TRUE);

    /** 2. Initialize the Exosite Ready SDK */
    /** SDK init needs to be done before the configurator server is started */
    exosite_sdk_init(platform_get_debug_printf(), NULL);

    /**
     * 3. Register the platform specific periodic function
     * @note It is necessary only if this is a non-threading system
     *       and there is a periodic task you have to call
     */
    exosite_sdk_register_periodic_fn(pl_init.platform_periodic_fn, pl_init.platform_fn_period);

    wifi_configured = platform_is_network_configured();
    if (!wifi_configured) {
        platform_connect_ap_mode();
        /* This is the IP address of WCM board when started in server mode */
        start_configurator_server("192.168.1.25", 80, NULL, on_new_wifi_config);

        while (!wifi_parameters_set) {
            exosite_delay_and_poll(exo, 50);
        }
        /*
         * We should wait until the response is sent to the browser
         * This is platform dependent so the application can decide how much to wait here
         *
         **/
        info_log(DEBUG_APPLICATION, ("Wait for before reset\n"));
        exosite_delay_and_poll(exo, 1000);
        info_log(DEBUG_APPLICATION, ("Perform reset\n"));
        info_log(DEBUG_APPLICATION, ("-------------\n"));
        platform_reset();
    }

    error = platform_get_network_configuration(&params);
    if (error)
        return;

    error = platform_join_network(&params);
    if (error)
       return;

    /** 4. Instantiate the Exosite object */
    error = exosite_new(&exo, "microchip", "dm182020wcm", NULL, APP_PROTO_HTTP);
    if (error)
      return;

    /** 5. Wait for the device activation */
    do {
        status = exosite_get_status(exo);
        exosite_delay_and_poll(exo, 200);
    } while (status != DEVICE_STATUS_ACTIVATED);

    /** 6. Subscribe to the "led" data source */
    error = exosite_subscribe(exo, "led", 0, on_change);
    if (error)
        return error;

    /** 7. Synchronize data sources with the server */
    exosite_read(exo, "led", on_read);

    start_time = system_get_time();
    /**
     * 8. In an infinite loop the app checks for
     *   - button press
     *   - sends the potmeter value at every 5 seconds
     */
    while (1) {
        for (i = 0; i < NUMBER_OF_BUTTONS; i++) {
            if (detect_edge(i) == TRUE) {
                info_log(DEBUG_NET, ("Button %d pressed\n", i));
                exosite_write(exo, switch_alias[i], "1", on_write);
            }
        }

        if (system_get_diff_time(start_time) > 5000) {
            start_time = system_get_time();
            platform_get_sensor_value(EXO_POTMETER, 0, potmeter_value_str, MAX_VALUE_SIZE);
            info_log(DEBUG_NET, ("Send potmeter value: %s\n", potmeter_value_str));
            exosite_write(exo, "pot", potmeter_value_str, on_write);
        }

        exosite_delay_and_poll(NULL, 10);
    }

}
