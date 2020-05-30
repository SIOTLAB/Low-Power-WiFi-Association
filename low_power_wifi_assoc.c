/*
 *  https://github.com/SIOTLAB/Low-Power-WiFi-Association
 */
#include <time.h>
#include <stdio.h>
#include "wiced.h"
#include "wwd_wifi.h"
#include "wwd_constants.h"
#include "besl_host.h"
#include "besl_host_interface.h"
#include "power_measurement.h" //HEADER file for EMPIOT



/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define MY_SSID "MY_SSID"
#define MY_KEY "MY_PASSWORD"


/*Different configurations supported*/

//#define JOIN_SPECIFIC_DHCP
//#define SCAN_JOIN_STATIC
#define SCAN_JOIN_DHCP
//#define SCAN_JOIN_STATIC


/*IP Configuration settings for STATIC IP ALLOCATION */
#ifdef SCAN_JOIN_STATIC || SCAN_JOIN_STATIC
#define IP_ADDRESS_STATIC "192.168.1.103"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"
#define CHANNEL "11"
#endif

#define AVG_LOOP_COUNT 50
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

int calculate_pmk( char* ssid, char* passkey, char* output_pmk );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               External Function Declarations
 ******************************************************/
extern besl_result_t besl_802_11_generate_pmk ( const char* password, const unsigned char* ssid, int ssid_length, unsigned char* output );


/******************************************************
 *               Function Definitions
 ******************************************************/

/* main application */
void application_start(void)
{
    /*Initializes the device and WICED framework */
    wiced_init();
    /*Initialize power measurement */
    init_power_measurement();

    wiced_time_t startime;
    wiced_time_t endtime;
    wiced_result_t result;
    char macStr[18];
    char pmk_hex[40];
    //mac address of the BSSID ( Acces point)
    int AP_MAC2[6] = {0x00,0xa0,0xb0,0xc0,0xd0,0xea};
    
    calculate_pmk(MY_SSID,MY_KEY,pmk_hex);

    snprintf(macStr, sizeof(macStr),"%02x:%02x:%02x:%02x:%02x:%02x", AP_MAC2[0],AP_MAC2[1],AP_MAC2[2],AP_MAC2[3],AP_MAC2[4],AP_MAC2[5]);
    wiced_rtos_delay_milliseconds(5000);
   
    /* Loop to connect and disconnect for an average power measurement */
    for(int i= 0; i < AVG_LOOP_COUNT ; ++i){
        wiced_time_get_time ( &startime );
       /*Start power measurement on EMPIOT */
        start_power_measurement();

            #ifdef JOIN_SPECIFIC_DHCP
                    result=wiced_wifi_join_specific( MY_SSID, strlen(MY_SSID), WICED_SECURITY_WPA2_MIXED_PSK, my_key, strlen(MY_KEY), macStr, CHANNEL, NULL, NULL, NULL);
            #endif
            #ifdef JOIN_SPECIFIC_STATIC
                    result=wiced_wifi_join_specific( MY_SSID, strlen(MY_SSID), WICED_SECURITY_WPA2_MIXED_PSK, my_key, strlen(MY_KEY), macStr, CHANNEL, IP_ADDRESS, NETMASK,GATEWAY);
            #endif
            #ifdef  SCAN_JOIN_DHCP
                    result =  wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
            #endif
            #ifdef  SCAN_JOIN_STATIC
                    result =  wiced_network_up(WICED_STA_INTERFACE, WICED_USE_STATIC_IP, NULL);
            #endif

        if( result != WICED_SUCCESS)
            WPRINT_APP_INFO(("\n\n\nERROR: JOIN SPECIFIC FAILED !!! \n\n\n"));
        
        end_power_measurement();
       /* compute total time taken to join */
        wiced_time_get_time ( &endtime );
        WPRINT_APP_INFO(("%d\n", (int)(endtime- startime)));
        wiced_rtos_delay_milliseconds(1000);
        WPRINT_APP_INFO(("Run #: %d\n", i));
        /* bring the interface down before next loop*/
        wiced_network_down(WICED_STA_INTERFACE);
    }
    WPRINT_APP_INFO(("DONE\n"));
}

/******************************************************
 *               Function Definitions
 ******************************************************/

int calculate_pmk( char* ssid, char* passkey, char* output_pmk )
{
    char pmk_hex[40];
    char pmk_char[65];
    if( strlen(ssid) > 32 || strlen(passkey) > 63 ||
            strlen(ssid) == 0 || strlen(passkey) == 0 )
    {
        printf("ERROR: SSID or PSK input error\n");
        return WICED_BADARG;
    }

    memset(pmk_hex, 0, sizeof(pmk_hex));
    memset(pmk_char, 0, sizeof(pmk_char));
    /* besl utility api to generate pmk /WICED/security/BESL/crypto_internal*/
    if ( besl_802_11_generate_pmk( passkey, (unsigned char *) ssid, strlen( ssid ), (unsigned char*) pmk_hex ) != 0 )
    {
        printf( "Error: Failed to generate pmk\n" );
        return WICED_ERROR;
    }
    /*utility api to convert to ascii char from hex */
    besl_host_hex_bytes_to_chars( pmk_char, (uint8_t*)pmk_hex, 32 );

    output_pmk = pmk_char;

    return WICED_SUCCESS;
}

