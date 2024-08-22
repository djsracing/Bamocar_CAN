#include "bamocar_can.h"
void setup() {
  sendMsg.can_id = STD_TXID;
  sendMsg.can_dlc = STD_REQ_DLC;
  recievedMsg.can_id = STD_RXID;
  Serial.begin(115200);
  while (!Serial)
    Serial.println("Error ");
  mcp.reset();
  mcp.setBitrate(STD_BAUD_RATE);
  mcp.setNormalMode();
  Serial.println("Hakuna Matata");
  sendMsg.data[0] = 0x3d;
  sendMsg.data[2] = 0x00;
}
void loop() {
  int hex = 0;

  if (hex == 0) {
    hex = REG_I_MAXPK;
    sendMsg.data[1] = hex;
    mcp.sendMessage(&sendMsg);
  } 
  else if (hex == 1) {
    hex = REG_TEMP_MOTOR;
    sendMsg.data[1] = hex;
    mcp.sendMessage(&sendMsg);
  } 
  else if (hex == 2) {
    hex = REG_TEMP_IGBT;
    sendMsg.data[1] = hex;
    mcp.sendMessage(&sendMsg);
  } 
  else if (hex >= 3) {
    hex = REG_TEMP_AIR;
    sendMsg.data[1] = hex;
    mcp.sendMessage(&sendMsg);
    hex = 0;
  }

  if (mcp.readMessage(&recievedMsg) == MCP2515::ERROR_OK && recievedMsg.can_id == 0x181) {

    Serial.print(F("|RXID:"));
    Serial.print(recievedMsg.can_id, HEX);
    Serial.print(F("| |DLC: "));
    Serial.print(recievedMsg.can_dlc, HEX);
    Serial.print(F("| |DATA (HEX): "));

    for (int i = 0; i < recievedMsg.can_dlc; i++) {
      Serial.print(recievedMsg.data[i], HEX);
      Serial.print(" ");
    }
    if (recievedMsg.can_dlc > 0) {
      Serial.print(F("| |DATA (DEC): "));
      uint32_t num = 0;
      for (int i = recievedMsg.can_dlc - 1; i >= 1; i--) {  // Start from byte 1
        num |= static_cast<uint32_t>(recievedMsg.data[i]) << ((i - 1) * 8);
      }
      Serial.print(num);
    }
    Serial.println("|");
    ;
    Serial.println(F("Message Sent:| | ID: "));
    Serial.print(sendMsg.can_id, HEX);
    Serial.print(F("| |DLC: "));
    Serial.print(sendMsg.can_dlc, HEX);
    Serial.print(F("| |DATA (HEX): "));
    for (int i = 0; i < sendMsg.can_dlc; i++) {
      Serial.print(sendMsg.data[i], HEX);
      Serial.print(" ");
    }
    hex++;
  }
  else{
    Serial.println("No Messages Yet");

  }
}
