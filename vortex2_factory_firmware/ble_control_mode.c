#include "ble_control_mode.h"
#include "nrf_log.h"
#include "myqueue.h"
#include "motor_mode.h"
#include "mode_global.h"

#define   setBoardMode      0x00
#define   setControlMode    0x01
#define   setMotorMode      0x02
#define   getInformation    0x03
#define   setSpeakerMode    0x04


#define BLEC_STACK_SIZE   256
#define BLEC_PRIORITY   2

TaskHandle_t m_blec_thread;

static uint8_t motorFreq = 0; 

struct receviceData{
  uint8_t head0;
  uint8_t head1;
  uint8_t len;
  uint8_t cs;
  uint8_t id;
  uint8_t ctr;
  uint8_t data[10];
  uint8_t tail0;
  uint8_t tail1;
}rd,sd;


/*
static uint8_t data_cs  = 0;
static uint8_t data_len = 0;
static uint8_t data_ctr = 0;*/

static uint8_t left_status = motor_stop;
static uint8_t right_status = motor_stop;
static uint16_t left_speed = 0;
static uint16_t right_speed = 0;
static uint8_t left_value = 0;
static uint8_t right_value = 0;

static void data_clear(){
    rd.head0 = 0x00;
    rd.head1 = 0x00;
    rd.len   = 0x00;
    rd.cs    = 0x00;
    rd.id    = 0x00;
    rd.ctr   = 0x00;
    memset(rd.data,0,10);
    rd.tail0 = 0x00;
    rd.tail1 = 0x00;
}

static bool check_cs(uint8_t *data){
    uint32_t cs = 0;
    if(rd.len > 0x05){
      cs += rd.id;
      cs += rd.ctr;
      for(uint8_t i=0; i < rd.len-5; i++){
        cs += rd.data[i];
      }
      cs += rd.tail0;
      cs += rd.tail1;
    }
    if((cs & 0xFF) == rd.cs){
      NRF_LOG_INFO("cs true");
      return true;
    }
    NRF_LOG_INFO("cs err read cs = 0x%X, count cs = 0x%X",rd.cs,cs);
    return false;
}
/*
static void runCommand(uint8_t *data){
    if(data_len >= 6 ){
       left_value = data[5];
       right_value = data[6];
       if(left_value >=0x00 && left_value <= 0x7F){
          left_status = motor_advance;
          left_speed = (uint16_t)1000*((left_value)/(float)127);
       }else{
          left_status = motor_retreat;
          left_speed = (uint16_t)1000*((left_value - 0x80)/(float)127);
       }

       if(right_value >= 0x00 && right_value <= 0x7F){
          right_status = motor_advance;
          right_speed = (uint16_t)1000*((right_value)/(float)127);
       }else{
          right_status = motor_retreat;
          right_speed = (uint16_t)1000*((right_value - 0x80)/(float)127);
       }
       left_motor(left_status, left_speed);
       right_motor(right_status, right_speed);
       left_speed = 0;
       right_speed = 0;
    }
}*/

static void AnalysisData(uint8_t *data){
  if(data[2] > 0x05){
    rd.head0 = data[0];
    rd.head1 = data[1];
    rd.len   = data[2];
    rd.cs    = data[3];
    rd.id    = data[4];
    rd.ctr   = data[5];
    memcpy(rd.data,data+6,(rd.len - 5) );
    rd.tail0 = data[rd.len+1];
    rd.tail1 = data[rd.len+2];
    NRF_LOG_INFO("rd.head0 = 0x%02X",rd.head0);
    NRF_LOG_INFO("rd.head1 = 0x%02X",rd.head1);
    NRF_LOG_INFO("rd.len = 0x%02X"  ,rd.len);
    NRF_LOG_INFO("rd.cs = 0x%02X"   ,rd.cs);
    NRF_LOG_INFO("rd.id = 0x%02X"   ,rd.id);
    NRF_LOG_INFO("rd.ctr = 0x%02X"  ,rd.ctr);
    for(uint8_t i =0; i < rd.len-5; i++){
      NRF_LOG_INFO("data[%d]=0x%02X",i,rd.data[i]);
    }
    NRF_LOG_INFO("rd.tail0 = 0x%02X",rd.tail0);
    NRF_LOG_INFO("rd.tail1 = 0x%02X",rd.tail1);

  }else{
    NRF_LOG_INFO("clear data");
    data_clear();
  }
}


static void getInformationFuc(void){
  sd.head0 = 0x55;
  sd.head1 = 0xAA;
  sd.len   = 0x00;
  sd.cs    = 0x00;
  sd.id    = 0x00;
  sd.ctr   = 0x03;
  sd.data[0] = nowBoard;
  sd.tail0 = 0x0D;
  sd.tail1 = 0x0A;

  sd.len   = 0x06;
  uint32_t cs = 0;
  cs = sd.id + sd.ctr + sd.data[0] + sd.tail0 + sd.tail1;
  sd.cs    = cs & 0xFF;
  uint8_t sendData[]={sd.head0, sd.head1, sd.len, sd.cs, sd.id,
                      sd.ctr, sd.data[0], sd.tail0, sd.tail1};
  send_ble_data(sendData,sizeof(sendData));
}

static void setControlModeFuc(void){
  run_mode = control_mode;
}

static void setBoardModeFuc(void){
  run_mode = board_mode;
}

static void setMotorModeFuc(void){
  if(run_mode == control_mode){
    motorFreq = 0;
    left_value  = rd.data[0];
    right_value = rd.data[1];
    if(left_value >=0x00 && left_value <= 0x7F){
      left_status = motor_advance;
      left_speed = (uint16_t)1000*((left_value)/(float)127);
    }else{
      left_status = motor_retreat;
      left_speed = (uint16_t)1000*((left_value - 0x80)/(float)127);
    }
    if(right_value >= 0x00 && right_value <= 0x7F){
       right_status = motor_advance;
       right_speed = (uint16_t)1000*((right_value)/(float)127);
    }else{
       right_status = motor_retreat;
       right_speed = (uint16_t)1000*((right_value - 0x80)/(float)127);
    }
    left_motor(left_status, left_speed);
    right_motor(right_status, right_speed);
    left_speed = 0;
    right_speed = 0;
  }
}

static void setSpeakerModeFuc(void){
  
}

static void AnalysisBleData(void){
    static struct BleMsg *p = NULL;
    p = cuappDequeue();
    if(p != NULL){
      AnalysisData(p->data);
      if((rd.head0 == 0x55) && (rd.head1 == 0xAA)){
        if(check_cs(p->data)){
          switch (rd.ctr)
          {
            case setControlMode:
              setControlModeFuc();
              break;
            case setBoardMode:
              setBoardModeFuc();
              break;
            case setMotorMode:
              setMotorModeFuc();
              break;
            case setSpeakerMode:
              setSpeakerModeFuc();
            case getInformation:
              getInformationFuc();
            default:
              break;
          }
        }else{//cs err
          data_clear();
        }
      }
      data_clear();
      free(p);
    }else{
    }
}


static void motor_clear(void){
  if((run_mode == control_mode) && motorFreq > 10){
    left_motor(motor_stop,0);
    right_motor(motor_stop, 0);
  }
  motorFreq ++;
  if(motorFreq  > 127){
    motorFreq = 10;
  }
}

static void blec_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  for(;;){
    if(ble_status == ble_connected){
      AnalysisBleData();
      motor_clear();
      vTaskDelay(20);
    }else{
      vTaskDelay(1000);
    }

  }
}

void blec_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(blec_thread,
                              "blec",
                              BLEC_STACK_SIZE,
                              NULL,
                              BLEC_PRIORITY,
                              &m_blec_thread));
}
