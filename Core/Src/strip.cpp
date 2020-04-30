#include <strip.h>

#define BIT_IS_SET(reg, bit) ((reg & (1 << bit)) != 0)

Strip::Strip(uint32_t leds_count, TIM_HandleTypeDef *tim_handler, uint32_t tim_channel, DMA_HandleTypeDef *dma_handler)
: leds_count(leds_count), tim_handler(tim_handler), tim_channel(tim_channel), dma_handler(dma_handler)
{
    // Allocate memory for leds
    this->leds = new Led [this->leds_count];
    this->set_timing_values();
    this->zero_delay_buffer();
}

Strip::~Strip()
{
    delete[] leds;
}

void Strip::set_red(uint8_t red, uint32_t index)
{
    this->leds[index].set_red(red);
}
void Strip::set_green(uint8_t green, uint32_t index)
{
    this->leds[index].set_green(green);
}
void Strip::set_blue(uint8_t blue, uint32_t index)
{
    this->leds[index].set_blue(blue);
}
void Strip::set_color(uint8_t red, uint8_t green, uint8_t blue, uint32_t index)
{
    this->leds[index].set_color(red, green, blue);
}
void Strip::set_string_color(uint8_t red, uint8_t green, uint8_t blue)
{
    for(uint32_t i = 0; i < this->leds_count; i++)
    {
        this->leds[i].set_color(red, green, blue);
    }
}
uint8_t Strip::get_red(uint32_t index) const
{
    return this->leds[index].get_red();
}
uint8_t Strip::get_green(uint32_t index) const
{
    return this->leds[index].get_green();
}
uint8_t Strip::get_blue(uint32_t index) const
{
    return this->leds[index].get_blue();
}

StripStatus Strip::get_status()
{
    return this->status;
}

void Strip::update()
{
    while (this->status != READY);  // Wait untill previous update ends
    this->set_status(SENDING_LEDS);

    this->write_led_in_buffer(0);
    if(this->leds_count > 1)
    {
        this->write_led_in_buffer(1);
    }
    this->transfer_counter++;

    HAL_TIM_PWM_Start_DMA(this->tim_handler, this->tim_channel, (uint32_t*) this->dma_circular_buffer, DMA_CIRCULAR_BUFFER_LENGTH);
}
void Strip::clear(){} // TODO

void Strip::dma_callback()
{
    if(this->get_status() == SENDING_DELAY)
    {
        if (this->delay_counter == 1)
        {
            HAL_TIM_PWM_Stop_DMA(this->tim_handler, this->tim_channel);
            this->transfer_counter = 0;
            this->set_status(READY);
        } else
        {
            this->delay_counter++;
        }
    }
    if(this->transfer_counter == this->leds_count)
    {
        HAL_TIM_PWM_Stop_DMA(this->tim_handler, this->tim_channel);
        this->set_status(SENDING_DELAY);
        this->send_delay();
    } else
    {
        this->transfer_counter++;
        if (this->transfer_counter < this->leds_count)
        {
            write_led_in_buffer(transfer_counter);
        }
    }
}

void Strip::send_delay()
{
    HAL_TIM_PWM_Start_DMA(this->tim_handler, this->tim_channel, (uint32_t*) this->dma_delay_buffer, DMA_CIRCULAR_BUFFER_LENGTH);
}

void Strip::set_timing_values()
{
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

void Strip::set_status(StripStatus status)
{
    this->status = status;
}

void Strip::write_led_in_buffer(uint32_t led_index)
{
    convert_color_byte(leds[led_index].get_green(), &dma_circular_buffer[dma_circular_buffer_index][0]);
    convert_color_byte(leds[led_index].get_red(), &dma_circular_buffer[dma_circular_buffer_index][7]);
    convert_color_byte(leds[led_index].get_blue(), &dma_circular_buffer[dma_circular_buffer_index][15]);
    if(dma_circular_buffer_index == 0)
    {
        dma_circular_buffer_index = 1;
    } else {
        dma_circular_buffer_index = 0;
    }
}

void Strip::convert_color_byte(uint8_t color, uint16_t buffer[])
{
    for (int i = 0; i < BITS_PER_COLOR; i++)
    {
        if(BIT_IS_SET(color, i))
        {
            buffer[BITS_PER_COLOR-i-1] = pwm_true;
        } else
        {
            buffer[BITS_PER_COLOR-i-1] = pwm_false;
        }
    }
}

void Strip::zero_delay_buffer()
{
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < BITS_PER_COLOR * COLORS_PER_LED; j++)
        {
            this->dma_circular_buffer[i][j] = 0;
        }
    }
}