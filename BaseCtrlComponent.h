
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Chrono.h>
#include <LightChrono.h>
#include <pt.h>
#include <string.h>
#include <vector>

//作为所有板子的基类，考虑与MAS无关，即所有日后的NCS

class BaseCtrlComponent {
   protected:
    unsigned long pinLed;  // LED不能少
    boolean needBlink;     // LED提示变量
    String boardId;
    String boardType;
    long timeBuffer;
    String strBuffer;



   public:
    BaseCtrlComponent(String bdId, String bdType);
    void setBoardId(String str);
    String getBoardId();
    void setBoardType(String str);
    String getBoardType();
    virtual void debugPrint(String str);  //用于开发中debug的显示
    virtual void sendMessage(String str);

    /*LED相关*/
    int threadBlinker(struct pt* pt);                               // blinker线程,通过needBlink异步控制LED
    int threadBlinkerOnce(struct pt* pt, int blinkDuration = 500);  // blinker线程,不阻塞主线程情况闪烁一次

    void setLedPin(unsigned long p);
    unsigned long getLedPin();
    void setNeedBlink(boolean b);
    boolean getNeedBlink();
    void changeLed(boolean b);
    void changeLed();

};