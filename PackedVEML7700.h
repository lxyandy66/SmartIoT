// 对于VEML 7700光线传感器的包装类

#pragma once
#include <Adafruit_VEML7700.h>
#include <CtrlBoardIndex.h>
#include <IOIndex.h>
#include <Wire.h>

class PackedVEML7700 : public VirtualAnalogReader {
 private:
  Adafruit_VEML7700* lightSensor;

 public:
  const static int READ_LUX=0;
  const static int READ_WHITE=1;
  const static int READ_ALS=2;
  PackedVEML7700(String id,String vName,Adafruit_VEML7700* ls,int smoothSize = 7): VirtualAnalogReader(1, 16, smoothSize) {
    // 这几个参数对应int port, int res, int smoothSize，无实际含义
    lightSensor = ls;
    this->setValueName(vName);
    this->setAcId(id);
  }

  void updateMeasurement() { setVirtualAnalog(lightSensor->readLux()); }

  void begin() { lightSensor->begin(); }
  void begin(TwoWire *theWire) { lightSensor->begin(theWire); }

  void updateMeasurement(int measurementMethod) {
    switch (measurementMethod) {
      case READ_LUX:
        /* code */
        setVirtualAnalog(lightSensor->readLux());
        break;
      case READ_WHITE:
        setVirtualAnalog(lightSensor->readWhite());
        break;
      case READ_ALS:
        setVirtualAnalog(lightSensor->readALS());
        break;
      default:
        setVirtualAnalog(lightSensor->readLux());
        break;
    }
  }

  double measureDirectly(int measurementMethod) {
    switch (measurementMethod) {
      case READ_LUX:
        /* code */
        return lightSensor->readLux();
        break;
      case READ_WHITE:
        return lightSensor->readWhite();
        break;
      case READ_ALS:
        return lightSensor->readALS();
        break;
      default:
        return lightSensor->readLux();
        break;
    }
  }

  void outputStatus(JsonDocument* jsonDoc) {
    (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
    (*jsonDoc)[this->valueName] = readAnalogDirectly(true);
  }

  void outputStatus(JsonDocument* jsonDoc, boolean onlyValue) {
    if (onlyValue) {
      (*jsonDoc)[this->valueName] = readAnalogDirectly(true);
      return;
    } else {
      outputStatus(jsonDoc);
    }
  }
  void outputStatus(JsonDocument* jsonDoc,boolean onlyValue,boolean needUpdate) {
    if (needUpdate)
      updateMeasurement();  // 此处是继承VirtualAnalogReader类，测量值需要额外更新
    if (onlyValue) {
      (*jsonDoc)[this->valueName] = readAnalogDirectly(
          true);  // VirtualAnalogReader类中，这个boolean参数没有用
      return;
    } else {
      outputStatus(jsonDoc);
    }
  }
};