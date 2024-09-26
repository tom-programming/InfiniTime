#pragma once

#include <cstdint>
#include <components/ble/HeartRateService.h>

namespace Pinetime {
  namespace Applications {
    class HeartRateTask;
  }
  namespace System {
    class SystemTask;
  }
  namespace Controllers {
    class HeartRateController {
    public:
      enum class States { Stopped, NotEnoughData, NoTouch, Running };
#ifndef SIM
      enum class LowPowerRecordStates { Stopped, Running };
#endif

      HeartRateController() = default;
      void Start();
      void Stop();
#ifndef SIM
      void StartLowPowerRecord();
      void StopLowPowerRecord();
#endif
      void Update(States newState, uint8_t heartRate);

      void SetHeartRateTask(Applications::HeartRateTask* task);
      States State() const {
        return state;
      }
#ifndef SIM
      LowPowerRecordStates LowPowerRecordState() const {
        return lowPowerRecordState;
      }
#endif
      uint8_t HeartRate() const {
        return heartRate;
      }

      void SetService(Pinetime::Controllers::HeartRateService* service);

    private:
      Applications::HeartRateTask* task = nullptr;
      States state = States::Stopped;
#ifndef SIM
      LowPowerRecordStates lowPowerRecordState = LowPowerRecordStates::Stopped;
#endif
      uint8_t heartRate = 0;
      Pinetime::Controllers::HeartRateService* service = nullptr;
    };
  }
}
