#pragma once
#include "CtrlBoardManager.h"
#include "VirtualAnalogIO.h"

class IoTCtrlBoardManager : public CtrlBoardManager {
    //CtrlBoardManager 的子类，实际使用于该项目中IoT控制板
    //实现跨网络的本地控制，即根据通过网络传递的传感器测量值进行本地控制
   protected:
    int takeOverTrigger;//用于接管原控制的数字输出针脚
    boolean isTakeOver = false;
    std::vector<VirtualAnalogReader*> virtualReaderContainer; //用于传感器的存储
    

   public:
   static const char* VIRTUAL_READ;
   static const char* VIRTUAL_READ_DATA;
//    static const char* VIRTUAL_READ;
//    static const char* VIRTUAL_READ;

   IoTCtrlBoardManager();
   IoTCtrlBoardManager(int takeOverTriggerPin);

   void setTakeOverTriggerPin(int pinNo);

   void defaultCommandDistributor(DynamicJsonDocument jsonBuffer,String cmdType);

   void addVirtualReader(VirtualAnalogReader* virtualAin);

   boolean checkReqOrder(long reqId);

   VirtualAnalogReader* findVirtualReaderById(String str);

   void showAccessoryStatus();
};
