#include "heartratetask/HeartRateTask.h"
#include <drivers/Hrs3300.h>
#include <drivers/SpiNorFlash.h>
#include <components/heartrate/HeartRateController.h>
#include <nrf_log.h>


Pinetime::Drivers::Hrs3300 *heartRateSensorMono;

#define BUFFER_DIM (5*2)

// Arduino driver
extern "C" {

#include <stdint.h>

float sqrt(float);
int __hardfp_sqrt(int f){ 
	//return sqrt(f);
	return 0; 
	} 

#define WHO_AM_3300  0x21
typedef enum {
	MSG_ALG_NOT_OPEN = 0x01,
	MSG_NO_TOUCH = 0x02,
	MSG_PPG_LEN_TOO_SHORT = 0x03,
	MSG_HR_READY = 0x04,
	MSG_ALG_TIMEOUT = 0x05,
	MSG_SETTLE = 0x06
} hrs3300_msg_code_t;

typedef enum {
	MSG_BP_ALG_NOT_OPEN = 0x01,
	MSG_BP_NO_TOUCH = 0x02,
	MSG_BP_PPG_LEN_TOO_SHORT = 0x03,
	MSG_BP_READY = 0x04,
	MSG_BP_ALG_TIMEOUT = 0x05,
	MSG_BP_SETTLE = 0x06
} hrs3300_bp_msg_code_t;
typedef struct {
  hrs3300_msg_code_t alg_status;
  uint32_t           data_cnt;
  uint8_t            hr_result;
  uint8_t            hr_result_qual;
	bool               object_flg;
} hrs3300_results_t;

typedef struct {
  hrs3300_bp_msg_code_t bp_alg_status;
  uint8_t            sbp;
  uint8_t            dbp; 
	uint32_t           data_cnt;
  uint8_t            hr_result;
	bool               object_flg;
} hrs3300_bp_results_t;

hrs3300_results_t Hrs3300_alg_get_results(void);
hrs3300_bp_results_t Hrs3300_alg_get_bp_results(void);

extern bool Hrs3300_alg_send_data(int16_t new_raw_data, int16_t als_raw_data, int16_t gsen_data_x, int16_t gsen_data_y, int16_t gsen_data_z, uint16_t timer_time);
extern bool Hrs3300_bp_alg_send_data(int16_t new_raw_data);

const uint8_t init_register_array[][2] = {
{0x01, 0xd0},  //11010000  bit[7]=1,HRS enable;bit[6:4]=101,wait time=50ms,bit[3]=0,LED DRIVE=22 mA
//{0x01, 0xf0},   //11010000  bit[7]=1,HRS enable;bit[6:4]=101,wait time=50ms,bit[3]=0,LED DRIVE=22 mA v13.05
{0x0c, 0x4e},  //00001110  bit[6]=0,LED DRIVE=22mA;bit[5]=0,sleep mode;p_pulse=1110,duty=50% 
{0x16, 0x78},  //01111000  bits[7:4]=0111,HRS 15bits
{0x17, 0x0d},  //00001101  bits[7:5]=011,HRS gain 16*;bit[1]=0,HRS data=AD0 
{0x02, 0x80},
{0x03, 0x00},
{0x04, 0x00},
{0x05, 0x00},
{0x06, 0x00},
{0x07, 0x00},
{0x08, 0x74},
{0x09, 0x00},
{0x0a, 0x08},
{0x0b, 0x00},
{0x0c, 0x6e},
{0x0d, 0x02},
{0x0e, 0x07},
{0x0f, 0x0f},
};
#define INIT_ARRAY_SIZE (sizeof(init_register_array)/sizeof(init_register_array[0]))

void Hrs3300_set_exinf(uint8_t age, uint8_t height, uint8_t weight , uint8_t gender , uint8_t ref_sbp , uint8_t ref_dbp);
extern bool Hrs3300_alg_open(void);
extern void Hrs3300_alg_close(void);
void Hrs3300_write_reg(uint8_t addr, uint8_t data);
uint8_t Hrs3300_read_reg(uint8_t addr);

uint8_t  hrs3300_bp_timeout_grade = 0; 
uint8_t  hrs3300_agc_init_stage = 0x04;
uint8_t  hrs3300_bp_power_grade = 1;
uint8_t  hrs3300_accurate_first_shot = 0;
uint8_t  hrs3300_up_factor = 3;
uint8_t  hrs3300_up_shift = 2;
uint16_t hrs3300_AMP_LTH = 120;
uint16_t hrs3300_hr_AMP_LTH = 150;
uint16_t hrs3300_hr_PVAR_LTH = 10;

static bool hrs3300_power_up_flg = 0 ;
uint8_t reg_0x7f ;
uint8_t reg_0x80 ;
uint8_t reg_0x81 ;
uint8_t reg_0x82 ;



uint16_t Hrs3300_read_hrs(void)
{
	uint8_t  databuf[3];
	uint16_t data;

	databuf[0] = Hrs3300_read_reg(0x09);
    databuf[1] = Hrs3300_read_reg(0x0a);
    databuf[2] = Hrs3300_read_reg(0x0f);
	
	data = ((databuf[0]<<8)|((databuf[1]&0x0F)<<4)|(databuf[2]&0x0F));

	return data;
}

uint16_t Hrs3300_read_als(void)
{
	uint8_t  databuf[3];
	uint16_t data;

	databuf[0] = Hrs3300_read_reg(0x08);
    databuf[1] = Hrs3300_read_reg(0x0d);	
    databuf[2] = Hrs3300_read_reg(0x0e);	
	
	data = ((databuf[0]<<3)|((databuf[1]&0x3F)<<11)|(databuf[2]&0x07));
	
	if (data > 32767) data = 32767; 

	return data;
}



bool Hrs3300_chip_init()
{
	int i =0 ;
	for(i = 0; i < INIT_ARRAY_SIZE;i++)
	{
	    Hrs3300_write_reg( init_register_array[i][0], init_register_array[i][1]);
  	}	
	
		if(hrs3300_power_up_flg == 0){
		  reg_0x7f=Hrs3300_read_reg(0x7f) ;
		  reg_0x80=Hrs3300_read_reg(0x80) ;
		  reg_0x81=Hrs3300_read_reg(0x81) ;
		  reg_0x82=Hrs3300_read_reg(0x82) ;		
			hrs3300_power_up_flg =  1; 
		}
	return true;
}

void Hrs3300_chip_enable()
{	
    Hrs3300_write_reg( 0x16, 0x78 );
    Hrs3300_write_reg( 0x01, 0xd0 );	
    Hrs3300_write_reg( 0x0c, 0x2e );
	
	return ;	
}

void Hrs3300_chip_disable()
{
	Hrs3300_write_reg( 0x01, 0x08 );
	Hrs3300_write_reg( 0x02, 0x80 );
	Hrs3300_write_reg( 0x0c, 0x4e );
	
	Hrs3300_write_reg( 0x16, 0x88 );
	
	Hrs3300_write_reg( 0x0c, 0x22 );
	Hrs3300_write_reg( 0x01, 0xf0 );
	Hrs3300_write_reg( 0x0c, 0x02 );

	Hrs3300_write_reg( 0x0c, 0x22 );
	Hrs3300_write_reg( 0x01, 0xf0 );
	Hrs3300_write_reg( 0x0c, 0x02 );
	
	Hrs3300_write_reg( 0x0c, 0x22 );
	Hrs3300_write_reg( 0x01, 0xf0 );
	Hrs3300_write_reg( 0x0c, 0x02 );
	
	Hrs3300_write_reg( 0x0c, 0x22 );
	Hrs3300_write_reg( 0x01, 0xf0 );
	Hrs3300_write_reg( 0x0c, 0x02 );
	
	return ;	
}

uint8_t get_heart_rate()
{
  int16_t new_raw_data,  als_raw_data;
  hrs3300_results_t alg_results;
  uint8_t n;
  hrs3300_bp_results_t  bp_alg_results ;
  static uint16_t timer_index = 0;
  new_raw_data = Hrs3300_read_hrs();
  als_raw_data = Hrs3300_read_als();
  Hrs3300_alg_send_data( new_raw_data,  als_raw_data, 0, 0, 0, 0);
  timer_index ++;
  if (timer_index >= 25)  {
    timer_index = 0;
    alg_results = Hrs3300_alg_get_results();
    if (alg_results.alg_status == MSG_NO_TOUCH)
    {
      return 254;
    }
    else if (alg_results.alg_status == MSG_PPG_LEN_TOO_SHORT)
    {
      return 253;
    }
    else
    {
      bp_alg_results = Hrs3300_alg_get_bp_results();
      return alg_results.hr_result;
    }
  }
  return 255;
}
}
//int __hardfp_sqrt(int f){ return sqrt(f); }
//}


extern "C" {
void Hrs3300_write_reg(uint8_t addr, uint8_t data) 
{
	if (heartRateSensorMono)
		heartRateSensorMono->WriteRegister(addr, data);
}

uint8_t Hrs3300_read_reg(uint8_t addr) 
{
   uint8_t reg_temp;
	if (heartRateSensorMono) {
		return heartRateSensorMono->ReadRegister(addr);
	}
	else return 0;
}

}

using namespace Pinetime::Applications;

HeartRateTask::HeartRateTask(Drivers::Hrs3300& heartRateSensor, 
                             Controllers::HeartRateController& controller, 
                             Controllers::FS& fs, 
                             Controllers::DateTime& dateTimeController,
                             Drivers::SpiNorFlash &spiNorFlash)
  : heartRateSensor {heartRateSensor}, controller {controller}, fs {fs}, dateTimeController {dateTimeController}, spiNorFlash {spiNorFlash}
  {
	 heartRateSensorMono = &heartRateSensor;
	 buffer = new uint8_t[BUFFER_DIM];
	 buffer_index = 0;
	 errorCode = 0;
}

void HeartRateTask::Start() {
  messageQueue = xQueueCreate(10, 1);
  controller.SetHeartRateTask(this);

  if (pdPASS != xTaskCreate(HeartRateTask::Process, "Heartrate", 500, this, 0, &taskHandle))
    APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
}

void HeartRateTask::Process(void* instance) {
  auto* app = static_cast<HeartRateTask*>(instance);
  app->Work();
}

void HeartRateTask::AddToBuffer(uint8_t meas, uint32_t time) {
  if (buffer_index*5 + 1 > BUFFER_DIM) {
	  vTaskPrioritySet(taskHandle, 2);
	  lfs_file_t hrsFile;
	  int err = fs.FileOpen(&hrsFile, "/hrs.dat", LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND);
    if (err != LFS_ERR_OK) { // there is an error -> do nothing
		  errorCode = err;
	  } else {
		  fs.FileWrite(&hrsFile, buffer, BUFFER_DIM);
		  fs.FileClose(&hrsFile);
	  }
	  // dummy read
	  /*int err2 = fs.FileOpen(&hrsFile, "/hrs.dat", LFS_O_RDONLY);
	  if (err2 == LFS_ERR_OK) {
      fs.FileRead(&hrsFile, buffer, BUFFER_DIM);
      fs.FileClose(&hrsFile);
	  }*/
	  vTaskPrioritySet(taskHandle, 0);
	  buffer_index = 0;
  }
  memcpy(&buffer[buffer_index*5], &time, 4);
  memcpy(&buffer[buffer_index*5+4], &meas, 1);
  buffer_index += 1;
}

void HeartRateTask::Work() {
  int lastBpm = 0;
  // FIX initialize lastMeasurement
  while (true) {
    Messages msg;
    uint32_t delay;
    // DOUBT: need to check the states here
    if (state == States::Running || lowPowerRecordState == LowPowerRecordStates::Running) {
      if (measurementStarted)
        delay = 40; // 40;
      else
        delay = 100; // 100;
    } else {
      delay = 100; // orig: portMAX_DELAY;
    }

    if (xQueueReceive(messageQueue, &msg, delay)) {
      switch (msg) {
        case Messages::GoToSleep:
          if (lowPowerRecordState != LowPowerRecordStates::Running)
             StopMeasurement();
          state = States::Idle;
          break;
        case Messages::WakeUp:
          state = States::Running;
          if (measurementStarted && (lowPowerRecordState != LowPowerRecordStates::Running)) {
            lastBpm = 0;
            StartMeasurement();
          }
          break;
        case Messages::StartMeasurement:
          if (measurementStarted)
            break;
          lastBpm = 0;
          StartMeasurement();
          measurementStarted = true;
          break;
        case Messages::StopMeasurement:
          if (!measurementStarted)
            break;
          StopMeasurement();
          measurementStarted = false;
          break;
#ifndef SIM
        case Messages::SetLPROn:
          lowPowerRecordState = LowPowerRecordStates::Running;
          break;
        case Messages::SetLPROff:
          lowPowerRecordState = LowPowerRecordStates::Stopped;
          break;
#endif
      }
    }

    if (measurementStarted) {
      //ppg.Preprocess(static_cast<float>(heartRateSensor.ReadHrs()));
      //auto bpm = ppg.HeartRate();
      uint8_t bpm = get_heart_rate(); // 255: nothing, 254: no touch, 253: wait

      // FIXIT need to add more context here
      if (bpm == 255 || bpm == 254 || bpm == 253) {
		  bpm = 0;
	  }

      if (lastBpm == 0 && bpm == 0)
        controller.Update(Controllers::HeartRateController::States::NotEnoughData, 0);
      if (bpm != 0) {
        lastBpm = bpm;
        auto newMeasurement = dateTimeController.CurrentDateTime();
        auto duration = newMeasurement - lastMeasurement;
        unsigned int passed_secs = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  
        controller.Update(Controllers::HeartRateController::States::Running, bpm);
  
        if (passed_secs > 60) {
		  lastMeasurement = newMeasurement;   
		  unsigned int timePoint = std::chrono::duration_cast<std::chrono::seconds>(lastMeasurement.time_since_epoch()).count();   
          AddToBuffer(bpm, timePoint);
	     }	    
      }
    }
  }
}

HeartRateTask::~HeartRateTask() {
   delete [] buffer;
}

void HeartRateTask::PushMessage(HeartRateTask::Messages msg) {
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(messageQueue, &msg, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    /* Actual macro used here is port specific. */
    // TODO : should I do something here?
  }
}

void HeartRateTask::StartMeasurement() {
  //heartRateSensor.Enable();
  Hrs3300_chip_init();
  Hrs3300_chip_enable();
  Hrs3300_set_exinf(0, 0, 0, 0, 0, 0);
  Hrs3300_alg_open();
  vTaskDelay(100);
  //ppg.SetOffset(static_cast<float>(heartRateSensor.ReadHrs()));
}

void HeartRateTask::StopMeasurement() {
  Hrs3300_alg_close();
  Hrs3300_chip_disable();
//  heartRateSensor.Disable();
  vTaskDelay(100);
}

// c6 is new
// ea is old
