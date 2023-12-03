#include <avr/io.h>
#include <avr/delay.h>

// Time unit in ms
uint8_t TIME_UNIT = 200;

uint8_t DOT = 1;
uint8_t DASH = 3;

void wait_time_unit(uint8_t time_unit)
{
    while (time_unit--)
    {
        _delay_ms(TIME_UNIT);
    }
}

void blink_led(uint8_t time_unit)
{

    DDRB = 1; // PB0 as an output (sink)

    wait_time_unit(time_unit);

    DDRB = 0;
}

int main()
{
    while (1)
    {
        // SOS
        blink_led(DOT);
        blink_led(DOT);
        blink_led(DOT);

        blink_led(DASH);
        blink_led(DASH);
        blink_led(DASH);
    }
}
