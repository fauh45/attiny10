#include <avr/io.h>
#include <util/delay.h>

#define DDRB_SCL DDB0
#define DDRB_SDA DDB1

#define READ_SDA PINB & (1 << PINB1)

#define SCL_ON PORTB |= (1 << PORTB0)
#define SDA_ON PORTB |= (1 << PORTB1)

#define SCL_OFF PORTB &= ~(1 << PORTB0)
#define SDA_OFF PORTB &= ~(1 << PORTB1)

inline void dly() { __asm__("NOP"); };

#define ADDR 0b01111000

void i2c_start()
{
    SDA_ON;
    dly();
    SCL_ON;
    dly();
    SDA_OFF;
    dly();
    SCL_OFF;
    dly();
}

void i2c_end()
{
    SDA_OFF;
    dly();
    SCL_ON;
    dly();
    SDA_ON;
    dly();
}

/**
 * @brief Send data to i2c pins on PB0 and PB1, this function assume that SLC is off
 *
 * @param data 8bit data to be sent
 * @return ack status
 */
bool Tx(uint8_t data)
{
    for (uint8_t i = 8; i; i--)
    {
        // Send data from the most significant digit
        (data & 0x80) ? SDA_ON : SDA_OFF;
        // Move the data to the most significant digit
        data <<= 1;

        dly();
        // Make the peripheral read the data bit
        // Data on SDA should not change while SCL is ON
        SCL_ON;
        dly();
        // Get ready for the next bit
        SCL_OFF;
        dly();
    }

    SDA_ON;
    SCL_ON;
    dly();

    // Read the reply from peripheral
    // ACK is when the peripheral with the right address set SDA to low
    bool ack = !READ_SDA;
    SCL_OFF;

    return ack;
}

const uint8_t INIT_SEQUENCE_LEN = 26;

const uint8_t INIT_SEQUENCE[] = {
    0xAE,       // Display OFF
    0xA8, 0x1F, // set multiplex (HEIGHT-1): 0x1F for 128x32, 0x3F for 128x64
    0xD3, 0x00, // Display offset to 0
    0x40,       // Set display start line to 0
    0x8D, 0x14, // Charge pump enabled
    0x20, 0x00, // Memory addressing mode 0x00 Horizontal 0x01 Vertical
    0xDA, 0x02, // Set COM Pins hardware configuration to sequential
    0x81, 0x80, // Set contrast
    0xD9, 0xF1, // Set pre-charge period
    0xDB, 0x40, // Set vcom detect

    0x22, 0x00, 0x03, // Page min to max
    0x21, 0x00, 0x7F, // Column min to max

    0xA4, // Entire display ON  A5 Enable / A4 Disable

    0xAF // Display on
};

void clear_display()
{
    i2c_start();

    Tx(ADDR);
    Tx(0x40);

    for (uint8_t i = 0; i < 128; i++)
    {

        Tx(0x00);
        Tx(0x00);
        Tx(0x00);
        Tx(0x00);
    }

    i2c_end();
}

int main()
{
    ADMUX = ADC2;
    ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);

    DDRB = (1 << DDRB_SCL) | (1 << DDRB_SDA);

    _delay_ms(100);

    i2c_start();

    Tx(ADDR);
    Tx(0x00);

    for (uint8_t i = 0; i < INIT_SEQUENCE_LEN; i++)
    {
        Tx(INIT_SEQUENCE[i]);
    }

    i2c_end();

    bool inverse = false;

    for (;;)
    {
        // Start ADC conversion
        ADCSRA |= (1 << ADSC);
        // Wait until the conversion is done
        loop_until_bit_is_clear(ADCSRA, ADSC);

        i2c_start();
        Tx(ADDR);

        if (ADCL > 128)
        {
            Tx(0x00);

            inverse = !inverse;
            inverse ? Tx(0xA7) : Tx(0xA6);

            _delay_ms(250);
        }
        else
        {
            Tx(0x40);

            for (uint8_t i = 0; i < 128; i++)
            {
                if (i <= ADCL)
                {

                    Tx(0xFF);
                    Tx(0xFF);
                    Tx(0xFF);
                    Tx(0xFF);
                }

                else
                {
                    Tx(0x00);
                    Tx(0x00);
                    Tx(0x00);
                    Tx(0x00);
                }
            }
        }

        i2c_end();
    }
}