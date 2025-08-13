#pragma once
#include "CtrlAccessory.h"


class VirtualAnalogReader : public AnalogReader {
    //通过virtualAnalogRead来设定输出值
   private:
    double virtualAnalogRead;
    double initalValue = 0;//避免在初始过程中队列为空导致异常

   protected:
    double readAnalogTool() {
        this->movCacu.append((double)virtualAnalogRead);
        // Serial.println("Virtual Read:"+String(virtualAnalogRead,3));
        return virtualAnalogRead;
    }

   public:
    VirtualAnalogReader(int port, int res, int smoothSize) : AnalogReader(port, res, smoothSize) {
        this->movCacu.append(initalValue);//避免在初始过程中队列为空导致异常
    }

    double readAnalogSmoothly(boolean needUpdated,boolean needMapping,boolean needSmooth){
        //当前所有的VirtualAnalogReader均不进行任何处理直接返回
        return virtualAnalogRead;
    }

    double readAnalogByMapping(boolean needUpdated){
        //当前所有的VirtualAnalogReader均不进行任何处理直接返回
        return virtualAnalogRead;
    }

    double readAnalogDirectly(boolean needUpdated){
        return virtualAnalogRead;
    }

    double getVirtualAnalog() { return this->virtualAnalogRead; }

    virtual void setVirtualAnalog(double input) { this->virtualAnalogRead = input; }

    virtual void updateMeasurement() = 0;//纯虚无参方法用于其他子类继承

    void showParameters(){
        this->debugPrint(this->acId + ": "+String(virtualAnalogRead, 3));
        this->debugPrint(this->acId + " Queue size: "+this->movCacu.getWindowSize());
    }

    void outputStatus(JsonDocument* jsonDoc) {
        (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
        (*jsonDoc)[this->valueName] = CtrlAccessory::roundDecimal(this->virtualAnalogRead,3) ;
    }
    
};

class VirtualAnalogWriter : public AnalogWriter , public CtrlAccessory{
   private:
    double virtualAnalogOut;

   public:
    VirtualAnalogWriter(int port, int res) : AnalogWriter(port, res) {}
    void setVirtualAnalog(double input) { this->virtualAnalogOut = input; }

    void showParameters(){
        this->debugPrint(this->acId + ": "+String(virtualAnalogOut, 3));
    }

    void outputStatus(JsonDocument* jsonDoc) {
        (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
        (*jsonDoc)[this->valueName] = CtrlAccessory::roundDecimal(this->virtualAnalogOut,3) ;
    }
};