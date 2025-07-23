#pragma once

#include "AgentProtocol.h"
#include <ArduinoJson.h>

//仅作为提取相应字段数据的字段名
const int MSG_SIZE = 256;  //用于解析的json数组大小
const char* AgentProtocol::CMD_GET = ("GET");
const char* AgentProtocol::CMD_INIT = ("INIT");

const char* AgentProtocol::CMD_IDNT = ("IDNT");
const char* AgentProtocol::CMD_SEND = ("SEND");

// 以下字符串均从串口获得的JSON字符串中取出
//字段名
const char* AgentProtocol::DEV_ID_FROM_JSON = ("id");
const char* AgentProtocol::REQ_ID_FROM_JSON = ("rq");
const char* AgentProtocol::RESP_ID_FROM_JSON = ("rp");
const char* AgentProtocol::CMD_TYPE_FROM_JSON = ("cmd");
const char* AgentProtocol::COMPUTE_TIME_FROM_JSON = ("ct");
const char* AgentProtocol::DEV_TYPE_FROM_JSON = "tp";
const char* AgentProtocol::DATA_FROM_JSON = ("dt");
const char* AgentProtocol::DATA_ISCONV_FROM_JSON = "cv";
const char* AgentProtocol::DATA_LAMBDA_FROM_JSON = "lm";
const char* AgentProtocol::DATA_TEMP_FROM_JSON = "t";

const char* AgentProtocol::TYPE_COOLING_TOWER = "ct";
const char* AgentProtocol::TYPE_CHILLER = "chi";

const char* AgentProtocol::TYPE_COORDINATOR = "co";

AgentMsg AgentProtocol::parseFromString(String msg) {
    AgentMsg temp;
    DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);

    DeserializationError t = deserializeJson(jsonBuffer, msg);
    // Serial.println("Msg from deserial" + String(t.c_str()));
    if (t) {
        Serial.println("in parse: get error");
        Serial.println(t.c_str());
        return temp;
    }
    temp.boardId = jsonBuffer[AgentProtocol::DEV_ID_FROM_JSON].as<String>();
    temp.boardType = jsonBuffer[AgentProtocol::DEV_TYPE_FROM_JSON].as<String>();
    temp.cmdType = jsonBuffer[AgentProtocol::CMD_TYPE_FROM_JSON].as<String>();
    temp.compTime = jsonBuffer[AgentProtocol::COMPUTE_TIME_FROM_JSON].as<long>();
    temp.reqId = jsonBuffer[AgentProtocol::REQ_ID_FROM_JSON].as<long>();
    temp.respId = jsonBuffer[AgentProtocol::RESP_ID_FROM_JSON].as<long>();
    temp.data = jsonBuffer[AgentProtocol::DATA_FROM_JSON].as<String>();  //我一直怀疑这个json的解析出来对不对

    return temp;
}

//用于验证消息是否有效
boolean AgentProtocol::isVaildMsg(AgentMsg msg) {
    return !(msg.boardId == "" || msg.boardType == "" || msg.reqId < 0);
}