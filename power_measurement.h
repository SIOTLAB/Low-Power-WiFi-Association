
#ifndef POWER_MEAS_LIB
#define POWER_MEAS_LIB

// Power measurement API
// Use with EMPIOT board
// Copyright SIOTLAB
// Behnam Dezfouli

// Initializes the pins used for communication with the power measurement device
void init_power_measurement()
{
    WPRINT_APP_INFO (("INITIALIZED POWER MEASUREMENT!!!\n"));
    wiced_gpio_init (WICED_GPIO_17, OUTPUT_PUSH_PULL);
    wiced_gpio_init (WICED_GPIO_18, OUTPUT_PUSH_PULL);

    wiced_gpio_output_high( WICED_GPIO_17 );
    wiced_gpio_output_high( WICED_GPIO_18 );
}

void disable_power_measurement()
{
    WPRINT_APP_INFO (("DISABLE POWER MEASUREMENT!!!\n"));
    wiced_gpio_init (WICED_GPIO_17, INPUT_PULL_UP);
    wiced_gpio_init (WICED_GPIO_18, INPUT_PULL_UP);
}

// Commands the power measurement device to start measurement
void start_power_measurement()
{
    wiced_gpio_output_low( WICED_GPIO_17 );
    wiced_gpio_output_high( WICED_GPIO_17 );
}

// Commands the power measurement device to stop measurement
void end_power_measurement()
{
    wiced_gpio_output_low( WICED_GPIO_18 );
    wiced_gpio_output_high( WICED_GPIO_18 );
}

#endif
