#include "audio_capture.h"

#include <stm32u5xx_ll_tim.h>
#include <stm32u5xx_ll_adc.h>
#include <zephyr/irq.h>
#include <zephyr/arch/arm/irq.h>

// ── Buffer management ────────────────────────────────────────────
static int16_t _buffer[AUDIO_FRAME_SAMPLES];
static volatile size_t _buffer_pos = 0;
static volatile bool _ready = false;
static int32_t _bias = 2048;

// ── Timer ISR ────────────────────────────────────────────────────
static void _audio_timer_isr(const void * /*arg*/) {
  LL_TIM_ClearFlag_UPDATE(TIM6);

  if (_ready) return;

  uint32_t raw = LL_ADC_REG_ReadConversionData12(ADC1);
  int32_t centered = (int32_t)raw - _bias;
  if (centered < -32768) centered = -32768;
  if (centered > 32767) centered = 32767;
  _buffer[_buffer_pos++] = (int16_t)centered;

  if (_buffer_pos >= AUDIO_FRAME_SAMPLES) {
    _ready = true;
  }
}

// ── Audio Capture Implementation ─────────────────────────────────

bool audio_init() {
  // 1. Init analog pin + ADC via Arduino API
  analogRead(PIN_MIC_ANALOG);

  // 2. Enable TIM6 clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  __DSB();

  // 3. Configure TIM6 for 62.5 µs period (16 kHz)
  LL_TIM_SetPrescaler(TIM6, 0);       // CK_CNT = 160 MHz
  LL_TIM_SetAutoReload(TIM6, 9999);   // 10000 ticks → 62.5 µs
  LL_TIM_GenerateEvent_UPDATE(TIM6);  // Load shadow registers

  // 4. Calibrate DC bias (synchronous, before timer ISR is active)
  const int CAL_SAMPLES = 128;
  int32_t sum = 0;
  for (int i = 0; i < CAL_SAMPLES; i++) {
    sum += analogRead(PIN_MIC_ANALOG);
  }
  _bias = sum / CAL_SAMPLES;

  // 5. Switch ADC to continuous conversion
  LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);
  LL_ADC_REG_StartConversion(ADC1);

  // 6. Register ISR
  irq_connect_dynamic(TIM6_IRQn, 1, _audio_timer_isr, NULL, 0);
  arm_irq_priority_set(TIM6_IRQn, 1, 0);
  arm_irq_enable(TIM6_IRQn);

  // 7. Start TIM6 counter
  LL_TIM_EnableIT_UPDATE(TIM6);
  LL_TIM_EnableCounter(TIM6);

  _buffer_pos = 0;
  _ready = false;

  return true;
}

bool audio_ready() {
  return _ready;
}

int16_t* audio_get_frame() {
  if (!_ready) return nullptr;
  return _buffer;
}

void audio_clear_ready() {
  _ready = false;
  _buffer_pos = 0;
}
