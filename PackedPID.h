#pragma once
#include <PID_v1.h>
#include <ArduinoJson.h>
#include "AnalogReader.h"
#include "AnalogWriter.h"
#include "CtrlAccessory.h"

class PackedPID: public CtrlAccessory {
    // PID控制器的包装类，包装控制器的输入输出及设定点,直接与IO口进行交互
    // 实际上我就是不想让原来的PID的IO直接露出来
   
   private:
    double setPoint, ctrlInput, ctrlOutput,ctrlReadActualOut;  //作为直接与PID控制器交互的变量
    String ctrlSetPointName, ctrlInputName, ctrlOutputName,ctrlReadActualOutName;
    AnalogReader* inputPort;
    AnalogReader* readActualOutputPort;
    //这纯粹是为了方便输出，其实控制里面不需要，即读取当前执行器的情况，例如，ctrlOutput为阀门设定开度，ctrlReadOutput为阀门当前实际开度
    AnalogWriter* outputPort;
    boolean isCtrlByMapping;//实际上对于这个类而言不知道自己的输入输出是模拟信号还是真实的物理量
    boolean needSmooth;//这个变量控制是否需要平滑，通常如果网络介入底层控制，即传感器信号通过网络发送，则不需平滑；如果通过本地ADC采样则需要平滑

   public:
    PID pidController; //公有好了，外部方便更新
    static const char* TUNING_KP;
    static const char* TUNING_TI;
    static const char* TUNING_TD;

//默认不调参则PID为1,0,0
    PackedPID(AnalogReader* inPort,AnalogWriter* outPort,double initSetpoint,int ControllerDirection)
        : CtrlAccessory("C_DEF"),inputPort(inPort),outputPort(outPort),setPoint(initSetpoint),
        pidController(&(this->ctrlInput),&(this->ctrlOutput),&(this->setPoint),1,0,0,ControllerDirection) {
        isCtrlByMapping = true;
        this->ctrlSetPointName = "C_DEF_SP";
        this->ctrlInputName = "C_DEF_IN";
        this->ctrlOutputName = "C_DEF_OUT";
        this->ctrlReadActualOutName = "C_DEF_AOUT";
        this->needSmooth = true;
    }

    PackedPID(AnalogReader* inPort, AnalogWriter* outPort, double initSetpoint,double kp, double ki, double kd,int ControllerDirection)
        : CtrlAccessory("C_DEF"),inputPort(inPort), outputPort(outPort), setPoint(initSetpoint), 
        pidController(&(this->ctrlInput), &(this->ctrlOutput), &(this->setPoint), kp, ki, kd,ControllerDirection){
        isCtrlByMapping = true;
        this->ctrlSetPointName = "C_DEF_SP";
        this->ctrlInputName = "C_DEF_IN";
        this->ctrlOutputName = "C_DEF_OUT";
        this->ctrlReadActualOutName = "C_DEF_AOUT";
        this->needSmooth = true;
    }

    void needSmoothinCtrl(boolean needS) { this->needSmooth = needS; }
    
    void enablePidController(boolean enable) { pidController.SetMode((enable?1:0)); }
    
    void needCtrlByMapping(boolean needMapping){
        this->isCtrlByMapping = needMapping;
    }

    double updatedControl(){
        //每次计算时通过AnalogReader的平滑输出来更新PID控制器交互的变量
        this->ctrlInput = inputPort->readAnalogSmoothly(false, isCtrlByMapping,false);//needSmooth
        // Serial.println("From PID: ctrlInput: "+String(ctrlInput)+needSmooth?"Smoth Y":"Smth N");
        // Serial.println((VirtualAnalogReader*)inputPort->showAccessoryStatus());
        // Serial.println(inputPort->());
        pidController.Compute();
        if (isCtrlByMapping) {
            this->outputPort->outputAnalogByMapping(this->ctrlOutput);
        } else {
            this->outputPort->outputAnalogDirectly(this->ctrlOutput);
        }
        if(readActualOutputPort!=NULL){
            this->ctrlReadActualOut=readActualOutputPort->readAnalogSmoothly(false, isCtrlByMapping);
        }
        return this->ctrlOutput;
        
    }

    double getCtrlInput() { return this->ctrlInput; }
    double getCtrlOutput() { return this->ctrlOutput; }
    void setSetpoint(double sp) { this->setPoint = sp; }
    double getSetpoint() { return this->setPoint; }


    void setReadActualOutputPort(AnalogReader* readPort){
        this->readActualOutputPort = readPort;
    }

    String getCtrlSetPointName() { return this->ctrlSetPointName; }
    void setCtrlSetPointName(String spName) { this->ctrlSetPointName=spName; }
    String getCtrlInputName() { return this->ctrlInputName; }
    void setCtrlInputName(String inName) { this->ctrlInputName=inName; }
    String getCtrlOutputName() { return this->ctrlOutputName; }
    void setCtrlOutputName(String outName) { this->ctrlOutputName=outName; }
    String getCtrlReadActualOutName() { return this->ctrlReadActualOutName; }
    void setCtrlReadActualOutName(String outName) { this->ctrlReadActualOutName=outName; }

    void showParameters(){
        debugPrint("input: "+String(ctrlInput,2)+", output: "+String(ctrlOutput,2)+", setpoint: "+String(setPoint,2));
        debugPrint("kp: "+String(pidController.GetKp(),2)+", ti: "+String(pidController.GetKi(),2)+", td: "+String(pidController.GetKd(),2));
    }

    void tuningParameter(double k,double i,double d){
        pidController.SetTunings(k, i, d);
    }


    boolean tuningParameter(String jsonStr) {
        // JSON解析
        Serial.println(jsonStr);
        DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
        DeserializationError t = deserializeJson(jsonBuffer, jsonStr);
        if (t) {
            return false;
        }
        //检测指令类型
        //默认为二次函数，k，b
        pidController.SetTunings(jsonBuffer[TUNING_KP].as<double>(),
                                 jsonBuffer[TUNING_TI].as<double>(),
                                 jsonBuffer[TUNING_TD].as<double>());
        return true;
    }

    void setEnable(boolean needEnable){
        this->pidController.SetMode(needEnable ? 1 : 0);
    }

    void outputStatus(JsonDocument* jsonDoc) {
        (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
        (*jsonDoc)[this->ctrlSetPointName] = this->setPoint;
        (*jsonDoc)[this->ctrlInputName] = CtrlAccessory::roundDecimal(this->ctrlInput,3) ;
        (*jsonDoc)[this->ctrlOutputName] = CtrlAccessory::roundDecimal(this->ctrlOutput,2);
        if(readActualOutputPort!=NULL){
            (*jsonDoc)[this->ctrlReadActualOutName] = CtrlAccessory::roundDecimal(this->ctrlReadActualOut,2);
        }
    }
};

