#include "bamocar_can.h"
int32_t ARR_N_ACTUAL[8][8] = {0};
int32_t ARR_N_MAX[8][8] = {0};
int32_t ARR_N_CMD[8][8] = {0};
int32_t ARR_I_ACTUAL[8][8] = {0};
int32_t ARR_I_DEVICE[8][8] = {0};
int32_t ARR_I_200PC[8][8] = {0};
int32_t ARR_RAMP_ACC[8][8] = {0};
int32_t ARR_RAMP_DEC[8][8] = {0};
int32_t ARR_TEMP_MOTOR[8][8] = {0};
int32_t ARR_TEMP_IGBT[8][8] = {0};
int32_t ARR_TEMP_AIR[8][8] = {0};
MCP2515 mcp(STD_CSPIN);
struct can_frame sendMsg;
struct can_frame recievedMsg;

bool parse_and_store(struct can_frame& msg) {
  bool success = false;

  if (mcp.readMessage(&msg) == MCP2515::ERROR_OK && msg.can_id == 0x181) {
    uint8_t registerId = msg.data[0];
    uint32_t dataValue = 0;

    if (msg.can_dlc == 4) {
      dataValue = getReceived16Bit(msg);
      success = true;
    } else if (msg.can_dlc == 6) {
      dataValue = getReceived32Bit(msg);
      success = true;
    } else {
      return false;
    }

    if (success) {
      switch (registerId) {
        case REG_N_ACTUAL:
          static uint8_t pos1 = 0;
          ARR_N_ACTUAL[pos1 / 8][pos1 % 8] = dataValue;
          pos1 = (pos1 + 1) % 64;
          break;

        case REG_N_MAX:
          static uint8_t pos2 = 0;
          ARR_N_MAX[pos2 / 8][pos2 % 8] = dataValue;
          pos2 = (pos2 + 1) % 64;
          break;

        case REG_I_ACTUAL:
          static uint8_t pos3 = 0;
          ARR_I_ACTUAL[pos3 / 8][pos3 % 8] = dataValue;
          pos3 = (pos3 + 1) % 64;
          break;

        case REG_I_DEVICE:
          static uint8_t pos4 = 0;
          ARR_I_DEVICE[pos4 / 8][pos4 % 8] = dataValue;
          pos4 = (pos4 + 1) % 64;
          break;

        case REG_I_200PC:
          static uint8_t pos5 = 0;
          ARR_I_200PC[pos5 / 8][pos5 % 8] = dataValue;
          pos5 = (pos5 + 1) % 64;
          break;

        case REG_N_CMD:
          static uint8_t pos6 = 0;
          ARR_N_CMD[pos6 / 8][pos6 % 8] = dataValue;
          pos6 = (pos6 + 1) % 64;
          break;

        case REG_RAMP_ACC:
          static uint8_t pos7 = 0;
          ARR_RAMP_ACC[pos7 / 8][pos7 % 8] = dataValue;
          pos7 = (pos7 + 1) % 64;
          break;

        case REG_RAMP_DEC:
          static uint8_t pos8 = 0;
          ARR_RAMP_DEC[pos8 / 8][pos8 % 8] = dataValue;
          pos8 = (pos8 + 1) % 64;
          break;

        case REG_TEMP_MOTOR:
          static uint8_t pos9 = 0;
          ARR_TEMP_MOTOR[pos9 / 8][pos9 % 8] = dataValue;
          pos9 = (pos9 + 1) % 64;
          break;

        case REG_TEMP_IGBT:
          static uint8_t pos10 = 0;
          ARR_TEMP_IGBT[pos10 / 8][pos10 % 8] = dataValue;
          pos10 = (pos10 + 1) % 64;
          break;

        case REG_TEMP_AIR:
          static uint8_t pos11 = 0;
          ARR_TEMP_AIR[pos11 / 8][pos11 % 8] = dataValue;
          pos11 = (pos11 + 1) % 64;
          break;

        default:
          Serial.println(F("Invalid REGID"));
          success = false;
          break;
      }
      delay(20);
    }
  }

  return success;
}

bool requestData(uint8_t Register, uint8_t interval) {

  sendMsg.data[0] = 0x3D;  
  sendMsg.data[1] = Register;
  sendMsg.data[2] = interval;

  return mcp.sendMessage(&sendMsg) == MCP2515::ERROR_OK;
}

void print_readMsg(can_frame& receivedMsg){
   if (mcp.readMessage(&recievedMsg) == MCP2515::ERROR_OK) {
    if (recievedMsg.can_id == 0x181) {
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
    }
  }
}

int16_t getReceived16Bit(can_frame &recievedMsg) {
  int16_t returnValue;
  returnValue = recievedMsg.data[1];
  returnValue |= (recievedMsg.data[2] << 8);
  return returnValue;
}
int32_t getReceived32Bit(can_frame &recievedMsg) {
  int32_t returnValue;
  returnValue = recievedMsg.data[1];
  returnValue |= (recievedMsg.data[2] << 8);
  returnValue |= (recievedMsg.data[3] << 16);
  returnValue |= (recievedMsg.data[4] << 24);
  return returnValue;
}
bool requestCurrent(uint8_t interval) {
  bool success = true;  // Assume success initially
  if (!requestData(REG_I_ACTUAL, interval))
    success = false;

  if (!requestData(REG_I_DEVICE, interval))
    success = false;

  if (!requestData(REG_I_200PC, interval))
    success = false;

  return success;
}

bool requestTemp(uint8_t interval) {
  bool success = true;
  if (!requestData(REG_TEMP_MOTOR, interval))
    success = false;

  if (!requestData(REG_TEMP_AIR, interval))
    success = false;

  if (!requestData(REG_TEMP_IGBT, interval))
    success = false;

  return success;
}

