#include <strip.h>

Strip::Strip(uint32_t leds_count, TIM_HandleTypeDef *tim_handler, uint32_t tim_channel, DMA_HandleTypeDef *dma_handler)
: leds_count(leds_count), tim_handler(tim_handler), tim_channel(tim_channel), dma_handler(dma_handler)
{
    // Allocate memory for leds
    this->leds = new Led [this->leds_count];
    // Set ARR
    this->tim_handler->Instance->ARR = (HAL_RCC_GetHCLKFreq()/800000) - 1;
    // Variable for floating point calculations
    double floating_point_calcs;
    // Calculate false value
    floating_point_calcs = WS2812B_PWM_LOW_BIT_LENGTH*\
                (double)HAL_RCC_GetHCLKFreq()/970000000;
    // Set false
    this->pwm_false = (uint16_t)floating_point_calcs;
    // Calculate true value
    floating_point_calcs = WS2812B_PWM_HIGH_BIT_LENGTH*\
                (double)HAL_RCC_GetHCLKFreq()/970000000;
    // Set true
    this->pwm_true = (uint16_t)floating_point_calcs;
}

Strip::~Strip()
{
    delete[] leds;
}