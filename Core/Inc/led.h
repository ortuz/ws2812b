#ifndef LED_H
#define LED_H

#include <stdint.h>

class Led
{
private:
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

public:
    void set_red(uint8_t red);
    void set_green(uint8_t green);
    void set_blue(uint8_t blue);
    void set_color(uint8_t red, uint8_t green, uint8_t blue);
    uint8_t get_red() const;
    uint8_t get_green() const;
    uint8_t get_blue() const;
};

#endif