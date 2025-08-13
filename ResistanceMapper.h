#pragma once

#include "Mapper.h"

class ResistanceMapper : public Mapper {
 public:
  using Mapper::Mapper;
  double refResistance=1;  // 已知的参考电阻
  long refVolt=1;          // 总电压降

  void setRefResistance(double refR) { this->refResistance = refR; }
  void setRefVolt(long refV) { this->refVolt = refV; }

  double mapping(double input, boolean needLimit = true) {
    double result = 0;
    if (input > 0)
      result = refResistance * input / (refVolt - input);
    else
      return -999;
    return result;
  }



};