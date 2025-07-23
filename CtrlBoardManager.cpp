#pragma once
#include "CtrlBoardManager.h"
#include <vector>

//cmd类型
const char* CtrlBoardManager::MAPPING_OPEARTION = "MAP";
const char* CtrlBoardManager::CTRL_SETPOINT = "CT_SP";
const char* CtrlBoardManager::CTRL_TUNING = "CT_TN";
const char* CtrlBoardManager::CTRL_ON = "CT_ON";

const char* CtrlBoardManager::COMP_ID = "id";

//本地控制器相关
const char* CtrlBoardManager::CTRL_SETPOINT_DATA = "sp";
const char* CtrlBoardManager::CTRL_DATA_ON = "on";

const char* CtrlBoardManager::MGR_STATUS = "STS";


const char* CtrlBoardManager::SER_OUT_LOOP="lp";


CtrlBoardManager::CtrlBoardManager() {}

long CtrlBoardManager::getLoopCount(){
    return *(this->loopCount);
}
void CtrlBoardManager::setLoopCount(long* loop){
    this->loopCount = loop;
}

void CtrlBoardManager::addMapper(Mapper* mp) {
    this->localReqId = -999;
    this->mapperContainer.push_back(mp);
}

void CtrlBoardManager::addController(PackedPID* controller){
    this->controllerContainer.push_back(controller);
}


Mapper* CtrlBoardManager::findMapperById(String str) {
    for (int i = 0; i < mapperContainer.size(); i++) {
        if (mapperContainer[i]->getAcId() == str)
            return mapperContainer[i];
    }
    return nullptr;
}

PackedPID* CtrlBoardManager::findControllerById(String str) {
    for (int i = 0; i < controllerContainer.size(); i++) {
        if (controllerContainer[i]->getAcId() == str)
            return controllerContainer[i];
    }
    return nullptr;
}


long CtrlBoardManager::commandDistributor(String str) {
    // 检测收到命令的类型，例如CMD+***，即检测前面CMD，并分配至相应的处理方法
    // 例如{cmd:"MAP",id:"FRM",dt:{k:2,b:1}}

    // JSON解析
    DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
    DeserializationError t = deserializeJson(jsonBuffer, str);
    if (t) {
        this->debugPrint("in parse: get error");
        Serial.println(t.c_str());
        return -999;
    }
    //检测指令类型
    String cmdType = jsonBuffer[AgentProtocol::CMD_TYPE_FROM_JSON].as<String>();
    long reqId = jsonBuffer[AgentProtocol::REQ_ID_FROM_JSON].as<long>();
    //检测收到的指令是否过期，避免乱序
    
    if(reqId==0){
        //无时序指令，例如接管，显示参数等
        
    }else if(!checkReqOrder(reqId)){
        // 过时数据，直接返回
        oldMsgProcess(jsonBuffer, reqId);
        return reqId;
    }else{
        //时序数据，更新Id

        this->localReqId = reqId;
    }

    //根据指令类型分配
    //还需要再封装一下，并设为虚函数，方便日后子类的继承
    if (cmdType == CtrlBoardManager::MAPPING_OPEARTION) {
        //读取到为映射器修改指令
        // 例如{cmd:"MAP",id:"FRM",dt:{k:2,b:1}}
        //mapper的参数可能会变化，即不一定是k,b，所以直接传dt进去好了
        // showAccessoryStatus();
        Mapper* changedMapper = findMapperById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if (changedMapper == NULL || changedMapper == nullptr) {
            debugPrint("Mapper not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return reqId;
        }
        //思路还是根据dt字段中JSON字符串传给Mapper自行处理
        boolean isSuccess = changedMapper->setParameter(jsonBuffer[AgentProtocol::DATA_FROM_JSON].as<String>());
        return reqId;

    } else if (cmdType == CtrlBoardManager::CTRL_SETPOINT) {
        // 例如{cmd:"CT_SP",id:"C_FR",sp:2.0}
        PackedPID* changedController=findControllerById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if(changedController == NULL || changedController == nullptr){
            debugPrint("Controller not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return reqId;
        }
        changedController->setSetpoint(jsonBuffer[CtrlBoardManager::CTRL_SETPOINT_DATA].as<double>());
        return reqId;
    } else if(cmdType == CtrlBoardManager::CTRL_TUNING){
        // 例如{cmd:"CT_TN",id:"C_FR",dt:{kp:2.0,ti:2.0,td:2.0}}
        PackedPID* changedController=findControllerById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if(changedController == NULL || changedController == nullptr){
            debugPrint("Controller not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return reqId;
        }
        changedController->tuningParameter(jsonBuffer[AgentProtocol::DATA_FROM_JSON].as<String>());
        return reqId;
    }  else {
        defaultCommandDistributor(jsonBuffer, cmdType);
        return reqId;
    }
    return reqId;
    // showAccessoryStatus();
}

void CtrlBoardManager::defaultCommandDistributor(DynamicJsonDocument jsonStr,String cmdType) {
    debugPrint("Null");
}

// std::vector<String> CtrlBoardManager::split(String str, String pattern) {
//     String::size_type pos;
//     std::vector<String> result;
//     str += pattern;  //扩展字符串以方便操作
//     int size = str.size();

//     for (int i = 0; i < size; i++) {
//         pos = str.find(pattern, i);
//         if (pos < size) {
//             String s = str.substr(i, pos - i);
//             result.push_back(s);
//             i = pos + pattern.size() - 1;
//         }
//     }
//     return result;
// }

void CtrlBoardManager::debugPrint(String str) {
    Serial.println(str);
}

double CtrlBoardManager::mappingValue(double originalValue, String mapperId) {
    Mapper* mp = findMapperById(mapperId);
    if (mp == NULL || mp == nullptr)
        return -999;
    return mp->mapping(originalValue);
}

void CtrlBoardManager::showAccessoryStatus(){
    for (int i = 0; i < mapperContainer.size();i++){
        Serial.println("Mapper id: " + mapperContainer[i]->getAcId() );
        mapperContainer[i]->showParameters();
    }
    for (int i = 0; i < controllerContainer.size();i++){
        Serial.println("Controller id: " + controllerContainer[i]->getAcId() );
        controllerContainer[i]->showParameters();
    }
}

double CtrlBoardManager::getSetpointById(String controllerId){
    for (int i = 0; i < controllerContainer.size();i++){
        if(controllerContainer[i]->getAcId()==controllerId)
            return controllerContainer[i]->getSetpoint();
    }
    return -999;
}

String CtrlBoardManager::showMeasuredStatus(boolean inJsonFormat){
    if(inJsonFormat)
        return measuredStatusFormat();
    //以下为实际用，直接用于测试中结果到处及数据处理
        // Serial.println("LoopCount:"+String(loopCount)+" rq " +String(reqId));
        // Serial.println("LoopCount:"+String(loopCount)+" Qr " +String(flowrateMeasure)); 
        // Serial.println("LoopCount:"+String(loopCount)+" Qs "+String(flowrateSetPoint));
        // Serial.println("LoopCount:"+String(loopCount)+" Vr " +String(valveOpening)); 
        // Serial.println("LoopCount:"+String(loopCount)+" Vs "+String(valveCtrl));
    // return ("LoopCount:" + String(loopCount) + 
    //         " rq " + String(reqId) + 
    //         " Qr " +String(flowrateMeasure) + 
    //         " Qs " + String(flowrateSetPoint) +
    //         " Vr " + String(valveOpening) + 
    //         " Vs " + String(valveCtrl));
    return "duludlu";
}
String CtrlBoardManager::measuredStatusFormat(){
    //JSON形式输出测量值
    String temp;
    jsonMeasureStatusOut.clear();
    jsonMeasureStatusOut[CtrlBoardManager::SER_OUT_LOOP]=*(this->loopCount);
    jsonMeasureStatusOut[AgentProtocol::REQ_ID_FROM_JSON]=this->localReqId;
    for (int i = 0; i < controllerContainer.size(); i++) {
        controllerContainer[i]->outputStatus(&jsonMeasureStatusOut);
    }
    serializeJson(jsonMeasureStatusOut, temp);
    return temp;
    // jsonOut[AgentProtocol::DEV_ID_FROM_JSON] = this->boardId;
}

boolean CtrlBoardManager::checkReqOrder(long reqId){
    return true;
}

void CtrlBoardManager::oldMsgProcess(DynamicJsonDocument jsonBuffer, long reqId){
    this->debugPrint("out-of-date msg received: "+ String(reqId));
}