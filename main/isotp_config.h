#ifndef __ISOTP_CONFIG__
#define __ISOTP_CONFIG__

/* Max number of messages the receiver can receive at one time, this value 
 * is affectied by can driver queue length
 */
#define ISO_TP_DEFAULT_BLOCK_SIZE   8

/* The STmin parameter value specifies the minimum time gap allowed between 
 * the transmission of consecutive frame network protocol data units
 */
#define ISO_TP_DEFAULT_ST_MIN       0

/* Maximum number of consecutive FC.Wait frames accepted while transmitting.
 * Each valid Wait frame refreshes N_Bs. Eight waits keep the transport bounded
 * while allowing a slow gateway/ECU to remain inside VW_Flash's 10 s response
 * window.
 */
#define ISO_TP_MAX_WFT_NUMBER       8

/* N_Bs/N_Cr timeout in microseconds. The previous 100 ms window was too short
 * for slow or busy vehicle networks and could abort a 4095-byte TransferData
 * request inside the dongle before the host saw an ECU response.
 */
#define ISO_TP_DEFAULT_RESPONSE_TIMEOUT 1000000

/* Private: Determines if by default, padding is added to ISO-TP message frames.
 */
#define ISO_TP_FRAME_PADDING

#endif
