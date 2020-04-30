#ifndef STRIP_H
#define STRIP_H

#include <stdint.h>
#include <led.h>

#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_dma.h>

#define WS2812B_PWM_HIGH_BIT_LENGTH 800
#define WS2812B_PWM_LOW_BIT_LENGTH 400
#define BITS_PER_COLOR 8
#define COLORS_PER_LED 3
#define DMA_CIRCULAR_BUFFER_LENGTH 2 * BITS_PER_COLOR * COLORS_PER_LED

enum StringStatus
{
    READY = 0,
    SENDING_LEDS,
    SENDING_DELAY
};

class Strip
{
private:
    uint32_t leds_count = 0;
    Led *leds;

    TIM_HandleTypeDef *tim_handler;
    uint32_t tim_channel;
    DMA_HandleTypeDef *dma_handler;

    uint16_t pwm_true;
    uint16_t pwm_false;

    uint32_t transfer_counter = 0;
    StringStatus status = READY;

    uint16_t dma_circular_buffer[DMA_CIRCULAR_BUFFER_LENGTH];

    void send_leds();
    void send_delay();

public:
    Strip(uint32_t leds_count, TIM_HandleTypeDef *tim_handler, uint32_t tim_channel, DMA_HandleTypeDef *dma_handler);
    ~Strip();

    void set_red(uint8_t red, uint32_t index);
    void set_green(uint8_t green, uint32_t index);
    void set_blue(uint8_t blue, uint32_t index);
    void set_color(uint8_t red, uint8_t green, uint8_t blue, uint32_t index);
    void set_string_color(uint8_t red, uint8_t green, uint8_t blue);
    uint8_t get_red(uint32_t index) const;
    uint8_t get_green(uint32_t index) const;
    uint8_t get_blue(uint32_t index) const;

    void clear();
    void update() const;

    void dma_callback();
};

#endif