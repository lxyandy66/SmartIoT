#pragma once
#include "AnalogReader.h"



AnalogReader::AnalogReader(int port, int res,int smoothSize):AnalogIOPort(port,res){
    movCacu=NumericMovingAverageFilter(smoothSize);
    pinMode(port, INPUT_ANALOG);
}

AnalogReader::AnalogReader(String id,String vName,int port,int res,int smoothSize):AnalogIOPort(port,res),CtrlAccessory(id,vName){
    movCacu=NumericMovingAverageFilter(smoothSize);
    pinMode(port, INPUT_ANALOG);
}

void AnalogReader::setSmoothWindowSize(int size){
    this->movCacu.setWindowSize(size);
}

double AnalogReader::readAnalogTool(){
    //直接跟IO交互的函数
    //读取的同时仍维持队列的更新
    double value=(double)analogRead(this->portNo);
    this->movCacu.append((double)value);
    return value;
}

void AnalogReader::updatedReadAnalog(){
    readAnalogTool();
}

//除了直接读取外，默认均不与IO口进行交互，仅通过队列处理
double AnalogReader:: readAnalogByMapping(boolean needUpdated){//读取模拟输出至百分比
    if(needUpdated)
        readAnalogTool();
    return mappingValue(this->movCacu.getNewestElement());
}

double AnalogReader:: readAnalogDirectly(boolean needUpdated){//读取模拟输出至百分比
    if(needUpdated)
        readAnalogTool();
    return this->movCacu.getNewestElement();
}

double AnalogReader::readAnalogSmoothly(boolean needUpdated,boolean needMapping,boolean needSmooth){
    //作为总入口好了
    if(needUpdated)
        readAnalogTool();
    if(!needSmooth)
        return needMapping?readAnalogByMapping(needUpdated):readAnalogDirectly(needUpdated);
    return needMapping?mappingValue(this->movCacu.getAverage()):this->movCacu.getAverage();
}

double AnalogReader::mappingValue(double originalValue){
    return this->mapper->mapping(originalValue);
}

void AnalogReader::showParameters(){
    this->debugPrint(this->acId + ": "+String(this->movCacu.getNewestElement(), 3));
    this->debugPrint(this->acId + " Queue size: "+this->movCacu.getWindowSize());
}

void AnalogReader::outputStatus(JsonDocument* jsonDoc) {
    (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
    (*jsonDoc)[this->valueName] = readAnalogDirectly(false) ;
}

void AnalogReader::outputStatus(JsonDocument* jsonDoc,boolean onlyValue){
    //其实不需要这个
    return outputStatus(jsonDoc, onlyValue, false);
}

void AnalogReader::outputStatus(JsonDocument* jsonDoc,boolean onlyValue, boolean needUpdate){
    if(onlyValue){
        (*jsonDoc)[this->valueName] = readAnalogDirectly(needUpdate) ;
        return;
    }else{
        outputStatus(jsonDoc);
    }
}