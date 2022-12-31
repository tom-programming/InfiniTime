#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "components/fs/FS.h"
#include "components/datetime/DateTimeController.h"
#include <chrono>


// States: Idle, Running
// measurementsStarted?
// lastMeasurements

namespace Pinetime {
  namespace Drivers {
    class Hrs3300;
    class Hrs3300;
  }
  namespace Controllers {
    class HeartRateController;
    class SpiNorFlash;
  }
  namespace Applications {
    class HeartRateTask {
    public:
      enum class Messages : uint8_t { GoToSleep, WakeUp, StartMeasurement, StopMeasurement, 
#ifndef SIM
SetLPROn, SetLPROff 
#endif
		  };
      enum class States { Idle, Running };
#ifndef SIM
      enum class LowPowerRecordStates { Stopped, Running };
#endif
      explicit HeartRateTask(Drivers::Hrs3300& heartRateSensor, 
                             Controllers::HeartRateController& controller, 
                             Controllers::FS& fs, 
                             Controllers::DateTime& dateTimeController,
                             Drivers::SpiNorFlash& spiNorFlash
							 );
	  ~HeartRateTask();
      void Start();
      void Work();
      void PushMessage(Messages msg);

    private:
      Pinetime::Controllers::FS& fs;
      Pinetime::Controllers::DateTime &dateTimeController;
      static void Process(void* instance);
      void StartMeasurement();
      void StopMeasurement();
      void AddToBuffer(uint8_t meas, uint32_t epoch);

      TaskHandle_t taskHandle;
      QueueHandle_t messageQueue;
      States state = States::Running;
#ifndef SIM
      LowPowerRecordStates lowPowerRecordState =  LowPowerRecordStates::Stopped;
#endif
      Drivers::Hrs3300& heartRateSensor;
      Controllers::HeartRateController& controller;
      Drivers::SpiNorFlash& spiNorFlash;
      bool measurementStarted = false;
      uint8_t *buffer; //[5];
      uint8_t buffer_index;
      uint8_t errorCode;
#ifndef SIM
      bool measuringLP = false;
#endif
      int lastOpen = 0;
      std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> lastMeasurement;
    };

  }
}
