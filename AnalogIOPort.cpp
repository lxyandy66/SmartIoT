#pragma once
#include "AnalogIOPort.h"

AnalogIOPort::AnalogIOPort(int port) : portNo(port) {
    this->resolutionBit = 8;  //默认为8位
    setRange();
    analogReadResolution(this->resolutionBit);
    analogWriteResolution(this->resolutionBit);
}

AnalogIOPort::AnalogIOPort(int port, int res): portNo(port), resolutionBit(res) {
    setRange();
    analogReadResolution(this->resolutionBit);
    analogWriteResolution(this->resolutionBit);
}

int AnalogIOPort::getPortNo() {
    return this->portNo;
}

void AnalogIOPort::setResolutionBit(int res) {  
    //设置DAC的分辨率
    this->resolutionBit = res;
    setRange();
}

int AnalogIOPort::getMaxValue() {
    return outputMax;
}

void AnalogIOPort::setRange() {
    //直接用resolutionBit来调用，不开放对外直接设置接口，避免两者不统一
    this->outputMax = (2 << (this->resolutionBit - 1)) - 1;
    this->upper = this->outputMax;
    this->lower = 0;  //默认的输出最小值为0
}

double AnalogIOPort::limitRange(double value,double lower,double upper) {  //范围限制
    if (value >= lower && value <= upper)
        return value;
    else if (value > upper)
        return upper;
    else
        return lower;
}

//设置实际的输入输出限制
double AnalogIOPort::getUpper() {
    return this->upper;
}
void AnalogIOPort::setUpper(double up) {
    this->upper = up > outputMax ? outputMax : up;
}
double AnalogIOPort::getLower() {
    return this->lower;
}
void AnalogIOPort::setLower(double low) {
    this->lower = low > 0 ? low : 0;
}

void AnalogIOPort::setMapper(Mapper* mp){
    this->mapper = mp;
}

double AnalogIOPort::mappingValue(double originalValue,boolean needLimit){
    //默认按百分比输出
    return (originalValue - this->lower) / (this->upper - this->lower);
}
