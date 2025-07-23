#pragma once
#include "AnalogWriter.h"

AnalogWriter::AnalogWriter(int port, int res):AnalogIOPort(port,res){}

void AnalogWriter:: outputAnalogTool(double limitedValue) {
    // 最终执行模拟量输出的函数，确保传入的值已经是合法值
    analogWrite(this->portNo, limitedValue);
}

void AnalogWriter::outputAnalogByMapping(double outputValue) {//直接按照映射关系输出，具体输出值由内部函数确定
    this->actualOutputValue = mappingValue(outputValue);
    // Serial.println("Actual outSet: "+String(actualOut));
    outputAnalogTool(actualOutputValue);
    // outputAnalogTool(mappingValue(outputValue));
}

double AnalogWriter::mappingValue(double originalValue){
    return this->mapper->mapping(originalValue);
}

void AnalogWriter::outputAnalogDirectly(double outputValue,boolean withLimit){//直接按值，并且由输出最大值限定
    this->actualOutputValue = withLimit ? limitRange(outputValue, this->getLower(), this->getUpper()) : outputValue;
    outputAnalogTool(actualOutputValue);
}

double AnalogWriter::getOutputValve(){ return this->actualOutputValue; }