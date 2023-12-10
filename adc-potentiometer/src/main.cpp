#include <avr/io.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#define TIME_UNIT 1 // ms

void loop_time_unit(uint8_t unit_amount)
{
    for (uint8_t i = 0; i < unit_amount; i++)
    {
        _delay_ms(TIME_UNIT);
    }
}

int main()
{
    uint8_t adc_val;

    ADMUX = ADC1;                                                      // Select ADC MUX to PIN PB1
    ADCSRA = (1 << ADEN) | (1 << ADPS0); // Turn on ADC, set presclaer to div factor 128

    while (1)
    {
        ADCSRA |= 1 << ADSC; // Start the conversion

        loop_until_bit_is_clear(ADCSRA, ADSC); // Wait for the conversion to be complete

        adc_val = ADCL;

        if (adc_val < 32) // If the encoder is set to 1/4 full, just turn on the LED without blink
        {

            DDRB |= 1 << DDB0; // Turn PB0 to sink
        }
        else
        {
            loop_time_unit(adc_val);
            DDRB |= 1 << DDB0; // Turn PB0 to sink
            loop_time_unit(adc_val);
            DDRB &= ~(1 << DDB0); // Turn PB0 to Tri-state
        }
    }
}