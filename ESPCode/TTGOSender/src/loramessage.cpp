#include "loramessage.h"

LoraMessage::LoraMessage(String id, String dst, String cmd, String msg) {
    iId = id;
    iDst = dst; 
    iCmd = cmd;
    iMsg = msg;
}

LoraMessage::~LoraMessage() {
}

String LoraMessage::getMessage() {
   String json = "{";
   json += "\"i\":\"" + iId + "\"";                  // MsgId
   json += ",\"d\":\"" + iDst + "\"";    // Destination
   json += ",\"c\":\"" + iCmd + "\"";    // Command
   json += ",\"m\":\"" + iMsg + "\"";   // Message
   json +=  "}";
    return json;
}