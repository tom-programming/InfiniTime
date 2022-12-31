#include "components/heartrate/HeartRateController.h"
#include <heartratetask/HeartRateTask.h>
#include <systemtask/SystemTask.h>

using namespace Pinetime::Controllers;

void HeartRateController::Update(HeartRateController::States newState, uint8_t heartRate) {
  this->state = newState;
  if (this->heartRate != heartRate) {
    this->heartRate = heartRate;
    service->OnNewHeartRateValue(heartRate);
  }
}

void HeartRateController::Start() {
  if (task != nullptr) {
    state = States::NotEnoughData;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::StartMeasurement);
  }
}

void HeartRateController::Stop() {
  if (task != nullptr) {
    state = States::Stopped;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::StopMeasurement);
  }
}

#ifndef SIM
void HeartRateController::StartLowPowerRecord() {
  if (task != nullptr) {
    lowPowerRecordState = LowPowerRecordStates::Running;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::SetLPROn);
  }
}

void HeartRateController::StopLowPowerRecord() {
  if (task != nullptr) {
    lowPowerRecordState = LowPowerRecordStates::Stopped;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::SetLPROff);
  }
}
#endif

void HeartRateController::SetHeartRateTask(Pinetime::Applications::HeartRateTask* task) {
  this->task = task;
}

void HeartRateController::SetService(Pinetime::Controllers::HeartRateService* service) {
  this->service = service;
}
