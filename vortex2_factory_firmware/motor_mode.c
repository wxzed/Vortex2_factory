#include "motor_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrf_drv_pwm.h"

#define   M1A     24
#define   M1B     13
#define   M2A     31
#define   M2B     30



uint32_t M1A_DATA = 0;
uint32_t M1B_DATA = 0;
uint32_t M2A_DATA = 0;
uint32_t M2B_DATA = 0;

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);

#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;


static uint16_t const              m_motor_top  = 1000;//1ms
static uint16_t const              m_motor_step = 200;
static uint8_t                     m_motor_phase;
static nrf_pwm_values_individual_t m_motor_seq_values;

static nrf_pwm_sequence_t const    m_motor_seq =
{
    .values.p_individual = &m_motor_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_motor_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static void motor_handler(nrf_drv_pwm_evt_type_t event_type)
{

    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint16_t * p_channels = (uint16_t *)&m_motor_seq_values;
        p_channels[0] = M1A_DATA;
        p_channels[1] = M1B_DATA;
        p_channels[2] = M2A_DATA;
        p_channels[3] = M2B_DATA;
    }
}
static void motor(void){
  nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            M1A | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            M1B | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            M2A | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            M2B | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_motor_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, motor_handler));
    m_used |= USED_PWM(0);

    m_motor_seq_values.channel_0 = 0;
    m_motor_seq_values.channel_1 = 0;
    m_motor_seq_values.channel_2 = 0;
    m_motor_seq_values.channel_3 = 0;
    m_motor_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_motor_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
}

void motor_init(){
  motor();
}

void left_motor(uint8_t direction ,uint32_t speed)
{
    if(direction == motor_advance){
        M2A_DATA = speed;
        M2B_DATA = 0;
    }else if(direction == motor_retreat){
        M2A_DATA = 0;
        M2B_DATA = speed;
    }else{
        M2A_DATA = 0;
        M2B_DATA = 0;
    }
}

void right_motor(uint8_t direction ,uint32_t speed)
{
    if(direction == motor_advance){
        M1A_DATA = speed;
        M1B_DATA = 0;
    }else if(direction == motor_retreat){
        M1A_DATA = 0;
        M1B_DATA = speed;
    }else{
        M1A_DATA = 0;
        M1B_DATA = 0;
    }
}