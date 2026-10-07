#ifndef BRIDGE_H
#define BRIDGE_H

#include "driver/twai.h"
#include "ble_server.h"	/* send_message_t; transitively constants.h -> bool16 */

void		isotp_init();
void		isotp_deinit();
void		isotp_start_task();
void		isotp_stop_task();

void		bridge_connect();
void		bridge_disconnect();
void		bridge_received_ble(const void* src, size_t size);
int32_t		bridge_send_isotp(send_message_t *msg);
uint16_t	bridge_send_available();

/* ---- Raw CAN mode ----
 * Additive, opt-in mode. When enabled, the TWAI RX task forwards EVERY received
 * frame to the host (promiscuous sniffing) and host->device raw frames are
 * transmitted directly instead of going through the ISO-TP layer. Defaults OFF;
 * ISO-TP behavior is unchanged while raw mode is off. */
void		bridge_set_raw_mode(bool16 on);
bool16		bridge_raw_mode_enabled();
void		bridge_forward_raw_frame(const twai_message_t* msg);

#endif