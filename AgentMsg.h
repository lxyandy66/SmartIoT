#pragma once
#include<string.h>
#include<Arduino.h>
class AgentMsg {
	//*考虑一下这个是不是多余了*/

	//交互的AgentMsg，含所有完整的字段
	// 一个标准的agent消息
	//{"cmd":"SEND","id":"pump1","tp":"pump","rq":1,"rp":1,"ct":10,"dt":"{\"t\":20.1}"}
	//一个不标准的agent消息，dt字段应该用字符串
	//{"cmd":"SEND","id":"pump1","tp":"pump","rq":1,"rp":1,"ct":10,"dt":{"t":20}}


	// 一个标准的coordinator消息
	//{"id":"Co_1","tp":"co","cmd":"SEND","rq":3,"rp":-1,"dt":"{\"cv\":true,\"lm\":0.01}","ct":61}

public:
	String boardId;
	String boardType;
	String cmdType;
	long compTime;
	long reqId;
	long respId;
	String data;
};