#pragma once
#include<Arduino.h>
#include"AgentMsg.h"

class AgentProtocol {
	//本类用于处理从json消息中解析的内容
public:
	//CMD类型
	static const char* CMD_GET;
	static const char* CMD_INIT;

	static const char* CMD_IDNT;
	static const char* CMD_SEND;


	// 以下字符串均从串口获得的JSON字符串中取出
	static const char* DEV_ID_FROM_JSON;
	static const char* DEV_TYPE_FROM_JSON;
	static const char* REQ_ID_FROM_JSON;
	static const char* RESP_ID_FROM_JSON;
	static const char* CMD_TYPE_FROM_JSON;
	static const char* COMPUTE_TIME_FROM_JSON;
	static const char* DATA_FROM_JSON;

	//data字段对应的Json数组内含的数据类型
	static const char* DATA_ISCONV_FROM_JSON;
	static const char* DATA_LAMBDA_FROM_JSON;
	static const char* DATA_TEMP_FROM_JSON;

	//定义整个系统中的设备类型
	static const char* TYPE_COOLING_TOWER;
	static const char* TYPE_CHILLER;
	static const char* TYPE_COORDINATOR;

	//统一定义json数组的大小
	static const int MSG_SIZE = 256;//用于解析的json数组大小
	static const int DATA_SIZE = 128;//用于解析数据中data项的json数组大小

	static AgentMsg parseFromString(String msg);
	static String toJsonString(AgentMsg msg);
	static boolean isVaildMsg(AgentMsg msg);//用于验证消息是否有效
};