#pragma once
#include "IoTCtrlBoardManager.h"

const char* IoTCtrlBoardManager::VIRTUAL_READ = "VA_IN";
const char* IoTCtrlBoardManager::VIRTUAL_READ_DATA = "val";

IoTCtrlBoardManager::IoTCtrlBoardManager() : CtrlBoardManager() {}
IoTCtrlBoardManager::IoTCtrlBoardManager(int takeOverTriggerPin): CtrlBoardManager(), takeOverTrigger(takeOverTriggerPin) {}

void IoTCtrlBoardManager::setTakeOverTriggerPin(int pinNo) {
    this->takeOverTrigger = pinNo;
}

void IoTCtrlBoardManager::defaultCommandDistributor(DynamicJsonDocument jsonBuffer,String cmdType) {
    if (cmdType == "TO") {//    {cmd:"TO"}
        isTakeOver = !isTakeOver;
        Serial.println(isTakeOver ? "Ture" : "False");
        digitalWrite(takeOverTrigger, !isTakeOver);
        return;
    } else if (cmdType == CtrlBoardManager::CTRL_ON) {
        // 例如{cmd:"CT_ON",id:"C_FR",on:true}
        PackedPID* changedController = findControllerById(
            jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if (changedController == NULL || changedController == nullptr) {
            debugPrint("Controller not found according to: " +jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return;
        }
        isTakeOver = jsonBuffer[CtrlBoardManager::CTRL_DATA_ON].as<bool>();
        changedController->pidController.SetMode(isTakeOver);
        Serial.println(isTakeOver ? "Ture" : "False");
        digitalWrite(takeOverTrigger, !isTakeOver);
        return;
    } else if (cmdType == IoTCtrlBoardManager::VIRTUAL_READ) {
        // 例如 {cmd:"VA_IN",id:"VM_FR",rq:11,val:2.0}
        VirtualAnalogReader* virtualReader = findVirtualReaderById(
            jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if (virtualReader == NULL || virtualReader == nullptr) {
            debugPrint("Virtual Reader not found according to: " +jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return;
        }
        virtualReader->setVirtualAnalog(jsonBuffer[IoTCtrlBoardManager::VIRTUAL_READ_DATA].as<double>());
        virtualReader->updatedReadAnalog();
        return;
    }else if(cmdType==CtrlBoardManager::MGR_STATUS){
        // 例如{cmd:"STS"}
        this->showAccessoryStatus();
        return ;
    }
}

void IoTCtrlBoardManager::addVirtualReader(VirtualAnalogReader* virtualAin) {
    this->virtualReaderContainer.push_back(virtualAin);
}

boolean IoTCtrlBoardManager::checkReqOrder(long reqId) {
    //设为0避免接管等无时序命令被阻碍
    //包含等于，考虑一个reqId（即一个Label）中可能同时发送设定点和传感器测量值
    return reqId == 0 ? true : this->localReqId <= reqId;
}

VirtualAnalogReader* IoTCtrlBoardManager::findVirtualReaderById(String str) {
    for (int i = 0; i < virtualReaderContainer.size(); i++) {
        if (virtualReaderContainer[i]->getAcId() == str)
            return virtualReaderContainer[i];
    }
    return nullptr;
}


void IoTCtrlBoardManager::showAccessoryStatus(){
    CtrlBoardManager::showAccessoryStatus();
    // Serial.println("Showing VirtualReader");
    for (int i = 0; i < virtualReaderContainer.size(); i++) {
        Serial.println("VirtualReader id: " + virtualReaderContainer[i]->getAcId() );
        virtualReaderContainer[i]->showParameters();
    }
}