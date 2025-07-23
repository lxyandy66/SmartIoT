#pragma once
#include <ArduinoJson.h>
#include "Mapper.h"
#include "AgentProtocol.h"
#include <vector>
#include <PID_v1.h>
#include "PackedPID.h"

// 维护板子运行的类
// 映射器维护，PID维护，命令分配等
// 维护所有可通过网络信息更改的类

// {"cmd":"MAP","mpn":"2nd","dt":"{\"k\":2,\"b\":3}"}

class CtrlBoardManager {
   protected:
    std::vector<Mapper*> mapperContainer;
    std::vector<PackedPID*>controllerContainer;
    StaticJsonDocument<512> jsonOut;
    StaticJsonDocument<512> jsonMeasureStatusOut;
    long* loopCount;
    long localReqId;

   public:
    static const char* MAPPING_OPEARTION;
    static const char* CTRL_SETPOINT;
    static const char* CTRL_TUNING;
    static const int CMD_SIZE=512;
    static const char* COMP_ID;
    static const char* CTRL_DATA_ON;
    static const char* CTRL_ON;

    static const char* CTRL_SETPOINT_DATA;

    static const char* MGR_STATUS;

    static const char* SER_OUT_LOOP;
    
    CtrlBoardManager();

    long getLoopCount();
    void setLoopCount(long* loop);

    void addMapper(Mapper* mp);
    void addController(PackedPID* controller);

    Mapper* findMapperById(String str);
    PackedPID* findControllerById(String str);

    long commandDistributor(String str);
    virtual void defaultCommandDistributor(DynamicJsonDocument jsonStr,String cmdType);  //用于处理未来需要匹配的命令


    virtual void debugPrint(String str);
    double mappingValue(double originalValue, String mapperId);

    double getSetpointById(String controllerId);

    void showAccessoryStatus();
    virtual String showMeasuredStatus(boolean inJsonFormat=true);

    virtual String measuredStatusFormat();  // JSON形式输出测量值

    virtual boolean checkReqOrder(long reqId);
    virtual void oldMsgProcess(DynamicJsonDocument jsonBuffer, long reqId);//对旧消息的处理，此处仅提示，日后留用
};
