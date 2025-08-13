#pragma once
#include <ArduinoJson.h>
#include "AgentProtocol.h"
#include "CtrlAccessory.h"

class Mapper :public CtrlAccessory{
    //多项式类，可设置自动更新
   private:
    double* parameter;
    int order;
    double lowLim;
    double upperLim;

   public:
    Mapper():CtrlAccessory("M_DEF") {
        //默认无参则直接输出
        this->order = 0;
        parameter = new double[1]{1};
        this->lowLim = 0;
        this->upperLim = 4095;
    }

    Mapper(int o, String setAcId): CtrlAccessory(setAcId){
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
        this->lowLim = 0;
        this->upperLim = 4095;
    }

    // 直接对多项式参数进行初始化的方法，但不对传入参数合法性进行检查
    Mapper(int o, double* p, String setAcId): CtrlAccessory(setAcId) {
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
        //不对传入的p进行检查
        for (int i = 0; i < o + 1; i++) {
            parameter[i] = p[i];
        }
        this->lowLim = 0;
        this->upperLim = 4095;
    }
    // 直接对多项式参数进行初始化的方法，但不对传入参数合法性进行检查
    Mapper(int o, double* p, String setAcId,double lLim,double uLim): CtrlAccessory(setAcId) {
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
        //不对传入的p进行检查
        for (int i = 0; i < o + 1; i++) {
            parameter[i] = p[i];
        }
        this->lowLim = lLim;
        this->upperLim = uLim;
    }


    //设为虚函数，可被重载为其他映射关系
    virtual double mapping(double input,boolean needLimit=true) {
        double result = 0;
        double multiple = 1;
        for (int i = 0; i < (order + 1); i++) {
            for (int j = (order - i); j > 0; j--)
                multiple *= input;
            result += parameter[i] * multiple;
            multiple = 1;
        }
        if(!needLimit){
            return result;
        }else{
            return result<lowLim?lowLim:(result>upperLim?upperLim:result);
        }
    }

    //按实际的次数更新参数，例如一阶则pos=1，更改二阶方程中二阶参数为99则为2,99
    boolean updateParameter(int orderChange, double value) {
        if (orderChange < 0 || orderChange > this->order)
            return false;  //不小于0且不大于阶数
        parameter[orderChange - order] = value;
        return true;
    }

    //用于批量设置多项式的参数
    void setParameter(double* para) {
        for (int i = 0; i < (order + 1); i++) {
            parameter[i] = para[i];
        }
    }

    //用于通过通讯的json字符串设置参数
    boolean setParameter(String jsonStr) {
        // JSON解析
        Serial.println(jsonStr);
        DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
        DeserializationError t = deserializeJson(jsonBuffer, jsonStr);
        if (t) {
            return false;
        }
        //检测指令类型
        //默认为二次函数，k，b
        parameter[0] = jsonBuffer["k"].as<double>();
        parameter[1] = jsonBuffer["b"].as<double>();
        return true;
    }

    void showParameters() {
        for (int i = 0; i < (order + 1); i++) {
            this->debugPrint(String(parameter[i],3)+" ");
        }
    }

    void setLimit(double lower,double upper){
        this->lowLim=lower;
        this->upperLim = upper;
    }

    double cutInLimit(double input){
      return input < lowLim ? lowLim : (input > upperLim ? upperLim : input);
    }
};