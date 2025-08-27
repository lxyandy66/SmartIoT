#pragma once
#include <max6675.h>
#include "CtrlAccessory.h"

// 包装过的MAX6675热电偶类

class PackedMAX6675 : public VirtualAnalogReader {
 private:
  MAX6675 *thermalCouple;

 public:

  PackedMAX6675(String id, String vName,  MAX6675 *tc,int smoothSize=7) : VirtualAnalogReader(1,16,smoothSize)  {//这几个参数对应int port, int res, int smoothSize，无实际含义
    thermalCouple = tc ;
    this->setValueName(vName);
    this->setAcId(id);
  }

  void updateMeasurement() {
    setVirtualAnalog((double)thermalCouple->readCelsius());
  }
  
  void outputStatus(JsonDocument* jsonDoc) {
    (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
    (*jsonDoc)[this->valueName] = readAnalogDirectly(true) ;
}

void outputStatus(JsonDocument* jsonDoc,boolean onlyValue){
    if(onlyValue){
        (*jsonDoc)[this->valueName] = readAnalogDirectly(true) ;
        return;
    }else{
        outputStatus(jsonDoc);
    }
}
void outputStatus(JsonDocument* jsonDoc,boolean onlyValue,boolean needUpdate){
    if(needUpdate)
      updateMeasurement();//此处是继承VirtualAnalogReader类，测量值需要额外更新
    if (onlyValue) {
      (*jsonDoc)[this->valueName] = readAnalogDirectly(true);//VirtualAnalogReader类中，这个boolean参数没有用
      return;
    } else {
      outputStatus(jsonDoc);
    }
}
};