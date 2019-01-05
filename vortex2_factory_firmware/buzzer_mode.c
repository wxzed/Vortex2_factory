#include "buzzer_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrf_drv_pwm.h"


static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);

#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;

static uint16_t                    m_buzzer_top  = 3816;
static uint16_t const              m_buzzer_step = 200;
static uint8_t                     m_buzzer_phase;

static nrf_pwm_values_individual_t m_buzzer_seq_values;

static nrf_pwm_sequence_t const    m_buzzer_seq =
{
    .values.p_individual = &m_buzzer_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_buzzer_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static void buzzer_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint16_t * p_channels = (uint16_t *)&m_buzzer_seq_values;
        p_channels[0] = m_buzzer_top/2;
    }
}
static void buzzer(void){
  nrf_drv_pwm_config_t const config1 =
    {
        .output_pins =
        {
            buzzer_pin | NRF_DRV_PWM_PIN_INVERTED, // channel 0
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_buzzer_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm1, &config1, buzzer_handler));
    m_used |= USED_PWM(1);

    m_buzzer_seq_values.channel_0 = 0;
    m_buzzer_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm1, &m_buzzer_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
}

void playNote(uint16_t note)
{
  switch(note){
    case Do:
      m_buzzer_top = 3816;
      break;
    case Re:
      m_buzzer_top = 3401;
      break;
    case Mi:
      m_buzzer_top = 3030;
      break;
    case Fa:
      m_buzzer_top = 2865;
      break;
    case Sol:
      m_buzzer_top = 2551;
      break;
    case La:
      m_buzzer_top = 2273;
      break;
    case Si:
      m_buzzer_top = 2024;
      break;
    default:
      break;
  }
  nrf_drv_pwm_uninit(&m_pwm1);
  buzzer();
}

void noTone(){
  nrf_drv_pwm_uninit(&m_pwm1);
}
void tone(uint32_t data,uint16_t time){

  m_buzzer_top = (1000*1000)/data;
  nrf_drv_pwm_uninit(&m_pwm1);
  if(data == 0){
  }else{
    buzzer();
  }
  
  vTaskDelay(time);
}

void buzzer_init(){
  buzzer();
}
