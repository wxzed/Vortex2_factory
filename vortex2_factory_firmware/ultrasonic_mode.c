#include "ultrasonic_mode.h"
#include "mode_global.h"
#include "neopixel_mode.h"
#include "motor_mode.h"
#include "pca10056.h"

#include "nrf_log.h"
#include "stdlib.h"
#include "time.h"
#include "i2c_mode.h"





unsigned char txbuf[10] = {0};

unsigned char addr0 = 0x11;

typedef enum {

  SLAVEADDR_INDEX = 0,
  PID_INDEX,
  VERSION_INDEX ,

  DIST_H_INDEX,
  DIST_L_INDEX,

  TEMP_H_INDEX,
  TEMP_L_INDEX,

  CFG_INDEX,
  CMD_INDEX,
  REG_NUM

} regindexTypedef;

#define    MEASURE_MODE_PASSIVE    (0x00)
#define    MEASURE_RANG_500        (0x20)
#define    CMD_DISTANCE_MEASURE    (0x01)

#define ULTRASONIC_STACK_SIZE   256
#define ULTRASONIC_PRIORITY   2

TaskHandle_t m_ultrasonic_thread;


static void ultrasonic_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  //srand((unsigned)(time(NULL)));
  ret_code_t err_code;
  uint8_t address;
  uint8_t sample_data;
  bool detected_device = false;
  bool checkStatus = false;
  setBreatheColor(R);
  twi_init();
  motor_init();
  while(1){
    if((run_mode == control_mode) && (ble_status == ble_connected)){
        vTaskDelay(1000);
    }else{
        uint16_t  dist, temp;
        txbuf[0] = CMD_DISTANCE_MEASURE;
        i2cWriteBytes(addr0, CMD_INDEX , &txbuf[0], 2 );
        vTaskDelay(50);
        dist = i2cReadBytes(addr0, DIST_H_INDEX , 2 );
        NRF_LOG_INFO("dist=%d cm",dist);
        if(dist > 35){
            checkStatus = false;
            left_motor(motor_advance,1000);
            right_motor(motor_advance,1000);
        }else{
            if(checkStatus){
            /*nothing */
            }else{
            if((rand()%2) == 0){
              /*left*/
              left_motor(motor_advance,1000);
              right_motor(motor_retreat,1000);
            }else{
              /*right*/
              left_motor(motor_retreat,1000);
              right_motor(motor_advance,1000);
            }
            checkStatus = true;
          }
        }
        dist = 0;
    }
  }
}

void ultrasonic_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(ultrasonic_thread,
                              "ultrasonic",
                              ULTRASONIC_STACK_SIZE,
                              NULL,
                              ULTRASONIC_PRIORITY,
                              &m_ultrasonic_thread));
}