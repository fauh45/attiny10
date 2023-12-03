#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB = 1 << DDB0 & ~(1 << DDB1); // Sets pin PB0 as output and PB1 as input
    PUEB = 1 << PUEB1;               // Activate internal pull up resistor in PB1

    uint8_t tick_pressed_cnt = 0; // Count how many ticks it has been since the button is long pressed

    while (1)
    {
        _delay_ms(10); // One tick

        switch (tick_pressed_cnt)
        {
        // Handle long press events, keep the lights on
        case 101:
            DDRB |= 1 << DDB0;

            // If the button is pressed, the button is setup to be pulled low
            // So high value in the register means it's not pressed
            if (!(PINB & (1 << PINB1)))
            {
                tick_pressed_cnt = 0; // Reset back after button being pushed
            }
            break;

        // Notify the user that long press is received
        case 100: // 100 * 10 = 1000ms, amount needed before moving to long-pressed
            DDRB &= ~(1 << DDB0);
            _delay_ms(1000);

            tick_pressed_cnt = 101; // Move to long press events
            break;

        default:
            // Get the PINB0 value out by checking the N-th bit value
            if (PINB & (1 << PINB1))
            // Button is not pressed
            {
                DDRB &= ~(1 << DDB0); // PB0 as an Tri-State (high impedance)

                tick_pressed_cnt = 0;
            }
            else
            // Button is pressed
            {
                DDRB |= (1 << DDB0); // PB0 as an output (sink)

                tick_pressed_cnt += 1; // Add count while the button is being long pressed
            }
            break;
        }
    }
}