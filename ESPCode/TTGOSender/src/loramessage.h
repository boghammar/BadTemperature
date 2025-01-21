/**
 * @file loramessage.h
 * @author Firstname Lastname
 * @brief structs and defintions for the Summertime LoRa message
 * @version 0.1
 * @date 2025-01-20
 * 
 * @copyright Copyright (c) boghammar.com 2025
 *
 */

#ifndef LORAMESSAGE_H
#define LORAMESSAGE_H

#include "wstring.h"

#define CMD_ERROR_REPORT "E"
#define CMD_SENSOR_DATA  "S"

#define GATEWAY_ID 1

class LoraMessage {
public:
    LoraMessage(String id, String dst, String cmd, String msg);
    ~LoraMessage();

    String getMessage();

private:
    String iId;
    String iDst; 
    String iCmd;
    String iMsg;
};

#endif