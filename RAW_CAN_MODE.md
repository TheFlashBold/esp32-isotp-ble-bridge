# Raw CAN Mode — Wire Protocol

Raw CAN mode is an **additive, opt-in** mode of the ESP32 BLE↔ISO-TP bridge. It lets
a host (VW_Flash GUI or a script) send and receive **raw 8-byte CAN frames**,
including **extended 29-bit IDs**, so the host can talk to non-ISO-TP channels such
as a VW EPS proprietary CCP/XCP-on-CAN measurement channel.

- **Default: OFF.** At boot the bridge behaves exactly as before (ISO-TP only).
- While raw mode is ON:
  - **TX:** host→device raw frames are transmitted verbatim on the bus (bypassing ISO-TP).
  - **RX:** the TWAI receive task forwards **every** frame on the bus to the host
    (promiscuous), each with its full arbitration ID, IDE flag, DLC and data bytes.
    ISO-TP routing is skipped while raw mode is on.
- The ISO-TP / flashing path is untouched. Turning raw mode OFF restores normal ISO-TP
  behavior immediately.

The CAN bitrate is unchanged (project `CAN_TIMING`, 500 kbit/s). The acceptance filter
is already `TWAI_FILTER_CONFIG_ACCEPT_ALL()`, so promiscuous RX needs **no** filter
change and there is nothing to save/restore.

---

## Transport framing (unchanged)

All messages use the existing 8-byte BLE/UART bridge header, little-endian:

```
offset size field
0      1    hdID     = 0xF1  (BLE_HEADER_ID)
1      1    cmdFlags
2      2    rxID     (LE16)
4      2    txID     (LE16)
6      2    cmdSize  (LE16)  = length of the payload that follows
8      N    payload  (cmdSize bytes)
```

Host→device packets are written to data characteristic **0xABF1**; device→host packets
arrive as notifications on **0xABF2**. The split-packet mechanism
(`BLE_COMMAND_FLAG_SPLIT_PK`, `0xF2` partial frames) is unchanged — raw frames are tiny
(≤ 14 bytes total) and never need splitting.

Relevant `cmdFlags` bits:

| bit  | value | name                         |
|------|-------|------------------------------|
| 4    | 0x10  | `BLE_COMMAND_FLAG_RAW`       |
| 6    | 0x40  | `BLE_COMMAND_FLAG_SETTINGS_GET` |
| 7    | 0x80  | `BLE_COMMAND_FLAG_SETTINGS`  |

---

## 1. Enter / exit raw mode (settings command)

Raw mode is toggled with a standard **setting**, id **9** (`BRG_SETTING_RAW_MODE`).

### Set (enter/exit)
```
hdID     = 0xF1
cmdFlags = 0x80 | 9           = 0x89   (SETTINGS | RAW_MODE)
rxID     = 0x0000
txID     = 0x0000
cmdSize  = 0x0001
payload  = [ 0x01 ]   -> enable raw mode
           [ 0x00 ]   -> disable raw mode
```
No reply is sent for a SET.

### Get (query current state)
```
hdID     = 0xF1
cmdFlags = 0x80 | 0x40 | 9    = 0xC9   (SETTINGS | SETTINGS_GET | RAW_MODE)
rxID     = 0x0000
txID     = 0x0000
cmdSize  = 0x0000
```
Device replies (notification on 0xABF2) with:
```
hdID     = 0xF1
cmdFlags = 0x80 | 9           = 0x89
rxID     = 0x0000
txID     = 0x0000
cmdSize  = 0x0001
payload  = [ state ]   0x00 = off, 0x01 = on
```

---

## 2. Raw TX (host → device)

Sent while raw mode is ON. If raw mode is OFF the frame is ignored (and a warning is
logged). `cmdFlags` has the RAW bit set; the CAN frame descriptor lives entirely in the
**payload** (the 16-bit `rxID`/`txID` header fields cannot hold a 29-bit ID and are
unused — set them to 0).

```
hdID     = 0xF1
cmdFlags = 0x10               (BLE_COMMAND_FLAG_RAW)
rxID     = 0x0000             (ignored)
txID     = 0x0000             (ignored)
cmdSize  = 6 + DLC
payload:
  offset size field
  0      1    rawFlags   bit0 (0x01) = RAW_CAN_FLAG_EXTENDED: 1 = 29-bit ext ID, 0 = 11-bit std
  1      1    DLC        0..8
  2      4    arbID      arbitration ID, little-endian uint32 (11- or 29-bit)
  6      DLC  data       DLC data bytes
```

**Example — EPS CCP CONNECT to extended ID 0x07FC9600, 8 data bytes `FF 00 00 00 00 00 00 00`:**
```
F1 10 00 00 00 00 0E 00            <- header: RAW flag, cmdSize=0x000E (14)
01 08 00 96 FC 07                  <- rawFlags=EXT, DLC=8, arbID=0x07FC9600 (LE)
FF 00 00 00 00 00 00 00            <- 8 CAN data bytes
```

---

## 3. Raw RX (device → host)

While raw mode is ON the bridge forwards **every** received CAN frame as a notification
on 0xABF2. Same payload layout as raw TX, so the host can read the unknown response ID
and sniff the bus.

```
hdID     = 0xF1
cmdFlags = 0x10               (BLE_COMMAND_FLAG_RAW)
rxID     = 0x0000
txID     = 0x0000
cmdSize  = 6 + DLC
payload:
  0      1    rawFlags   bit0 = extended-ID flag
  1      1    DLC
  2      4    arbID      full arbitration ID, little-endian uint32
  6      DLC  data
```

Multiple raw-RX frames may be coalesced into a single BLE notification by the existing
multi-send logic: each is a complete `header + payload` block, so the host must parse
the notification as a sequence of `hdID (0xF1) + 8-byte header + cmdSize payload` blocks
(exactly as it already does for normal bridge traffic).

> Note: promiscuous forwarding pushes frames onto the BLE send queue (64 deep). On a
> busy bus the queue can saturate and the oldest raw frames may be dropped — acceptable
> for targeted CCP/XCP sniffing. Enable raw mode only for the duration of the dump.

---

## Host-side quick recipe (EPS dump)

1. Enter raw mode: send the SET command (`0x89`, payload `0x01`).
2. Send CCP frames to ext ID `0x07FC9600` (CONNECT `0xFF`, SET-MTA `0xF6`,
   UPLOAD `0xF5`/`0xF4`) as raw TX frames.
3. Read all raw-RX notifications, discover the response arbitration ID from the
   forwarded frames, and reassemble the uploaded data.
4. Exit raw mode: send the SET command (`0x89`, payload `0x00`).
