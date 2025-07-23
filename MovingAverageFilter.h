#pragma once
#include <queue>

template <class T,class N>  // T 为每个元素的模版类，N为统计的模版类，例如，T可为AgentBuffer，N可为double
class MovingAverageFilter {
    //滑动平均值计算器，维护一个数据储存队列
    //作为基类，保留追加、计算、更新接口用于子类
    protected:
    std::queue<T> smoothQueue;  //该队列用于平滑模拟量读取的震荡，队列中保存原始值
    N sum;
    int windowSize;  //滑窗大小，决定队列容量

   public:
    MovingAverageFilter(int size=7) :windowSize(size){}
    N append(T ele) {
        smoothQueue.push(ele);
        if (smoothQueue.size() > windowSize) {
            T temp = smoothQueue.front();
            updateSum(ele, temp);  // sum-temp+ele
            smoothQueue.pop();
            return getAverage();
        }
        addSum(ele);
        return getAverage();
    }
    void setWindowSize(int s) { this->windowSize = s; }     //设置滑窗大小
    int getWindowSize() { return smoothQueue.size(); }      //查看滑窗大小
    T getNewestElement() { return smoothQueue.back(); }
    T getOldestElement() { return smoothQueue.front(); }
    virtual void updateSum(T appendEle, T deleteEle) = 0;   //容量超过，一个元素弹出，一个元素压入，此处更新求和
    virtual void addSum(T appendEle) = 0;                   //容量未超出，直接压入，此处求和
    virtual N getAverage() = 0;                             //根据现有求平均
    
};

class NumericMovingAverageFilter: public MovingAverageFilter<double, double> {
    public:
    NumericMovingAverageFilter(int size=7):MovingAverageFilter(size){}
    
    void updateSum(double appendEle, double deleteEle) {
        this->sum = this->sum + appendEle - deleteEle;
    }

    void addSum(double appendEle) {
        this->sum += appendEle;
    }
    double getAverage() { 
        return (double)this->sum / smoothQueue.size(); 
    }
};