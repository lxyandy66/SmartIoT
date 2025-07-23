#pragma once
#include <ArduinoJson.h>

class CtrlAccessory {
    //作为所有控制辅助模块的基类

    protected:
     String acId;
     String valueName;//测得的物理量的命名，可直接用于Json输出

    public:
     CtrlAccessory() { this->acId = "A_DEF"; this->valueName = "V_DEF";}
     CtrlAccessory(String id) { this->acId = id; this->valueName = "V_DEF";}
     CtrlAccessory(String id,String vName) { 
         this->acId = id;
         this->valueName = vName;
     }

     String getValueName() { return this->valueName; }
     void setValueName(String vName) { this->valueName = vName; }
     String getAcId() { return this->acId; }
     void setAcId(String str) { this->acId = str; }

     //用于将当前的数据输出到Json
     virtual void outputStatus(JsonDocument* jsonDoc) {}

     virtual void showParameters() = 0;

     virtual void debugPrint(String str) { Serial.println(str); }

     static double roundDecimal(double value,int precision=3) {
        int multiper = 1;
        for (int i = 0; i < precision;i++)
            multiper *= 10;
        return (int)(value * multiper + 0.5) / (multiper * 1.0);
    }

};