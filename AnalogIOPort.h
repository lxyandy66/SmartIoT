#pragma once
#include <Arduino.h>
#include "Mapper.h"

class AnalogIOPort {
//作为整个模拟量IO的基类，提供模拟量的分辨率限制，映射等

   protected:
    int portNo;

    int resolutionBit;
    int outputMax;  //根据DAC的位数决定输出的最大值
    
    double lower;
    double upper;   //实际使用的最大值

    Mapper* mapper;

    virtual double limitRange(double value,double lower,double upper);//将输入值限制在范围内，子类可自定义
    void setRange();

   public:
    AnalogIOPort(int port);
    AnalogIOPort(int port, int res);

    int getPortNo();

    //设置输出的上下限，默认与DAC的分辨率相同，即0-DAC分辨率
    //防止部分情况下读取的模拟量无法达到理想情况
    double getUpper();
    void setUpper(double up);
    double getLower();
    void setLower(double low);
    void setMapper(Mapper* mp);

    virtual double mappingValue(double originalValue,boolean needLimit);//映射关系函数，按照预定关系处理

    ///DAC设定，仅支持设定DAC位数，自动决定输出最大值
    int getMaxValue();
    void setResolutionBit(int res);
    
};
