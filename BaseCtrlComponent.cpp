#pragma once
#include"BaseCtrlComponent.h"

BaseCtrlComponent::BaseCtrlComponent(String bdId, String bdType) :boardId(bdId), boardType(bdType) {}

String BaseCtrlComponent::getBoardId() { return this->boardId; }

void BaseCtrlComponent::setBoardId(String str) { this->boardId = str; }

void BaseCtrlComponent::setBoardType(String str) { this->boardType = str; }

String BaseCtrlComponent::getBoardType() { return this->boardType; }

// BaseCtrlComponent::~BaseCtrlComponent() {}

int BaseCtrlComponent::threadBlinker(struct pt* pt) {
	//LED控制线程, 通过内置的boolean变量控制，常驻线程
	PT_BEGIN(pt);
	while (true) {
		PT_WAIT_UNTIL(pt, this->needBlink);//持续监听needBlink变量
		this->changeLed(true);
		// this->debugPrint("in the class, set led as on");
		PT_WAIT_UNTIL(pt, !(this->needBlink));
		this->changeLed(false);
		// this->debugPrint("in the class, set led as off");
	}
	PT_END(pt);
}

int BaseCtrlComponent::threadBlinkerOnce(struct pt* pt, int blinkDuration) {
	//LED控制线程，用于不阻塞主线程时闪烁
	PT_BEGIN(pt);
	long starter = millis();
	this->changeLed(true);
	PT_WAIT_UNTIL(pt, millis() - starter > blinkDuration);
	this->changeLed(false);
	// this->debugPrint("in the class, set led as off");
	PT_END(pt);
}

void BaseCtrlComponent::changeLed(boolean b) {
	digitalWrite(this->pinLed, b);
	return;
}

void BaseCtrlComponent::changeLed() {
	this->needBlink = !needBlink;
	digitalWrite(this->pinLed, this->needBlink);
	return;
}


void BaseCtrlComponent::setNeedBlink(boolean b) {
	this->needBlink = b;
}
boolean BaseCtrlComponent::getNeedBlink() {
	return this->needBlink;
}

void BaseCtrlComponent::setLedPin(unsigned long p) {
	pinMode(p, OUTPUT);
	this->pinLed = p;
}
unsigned long BaseCtrlComponent::getLedPin() { return this->pinLed; }

void BaseCtrlComponent::sendMessage(String msg){
    Serial.println(msg);
}