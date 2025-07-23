#pragma once
#include "VirtualAnalogIO.h"
//本程序包含虚拟物理系统, 用于控制相关的测试



class TestSystem {
   public:
    NumericMovingAverageFilter filter;
    double sysOutput;  //输出，被控对象状态

    TestSystem(double initOutput) { sysOutput = initOutput; }

    double updateState(double controllerOutput) {
        filter.append(sysOutput + 0.031 * (controllerOutput) -random(1, 3));
        sysOutput = filter.getAverage();  //(1 / (10 * s + 1))*exp((-2)*s);-random(0,2)
        return sysOutput;
    }
};