#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define OUT_TXD     DDRD |= _BV(1)
#define IN_RXD      DDRD &= _BV(0) 
#define UBRRVAL     12

void init_uart(){
    OUT_TXD;
    //IN_RXD;

    UBRRH = (unsigned char)(UBRRVAL >> 8);
    UBRRL = (unsigned char)(UBRRVAL);

    UCSRB |= (1 << RXEN) | (1 << TXEN);
    UCSRC |= (1 << URSEL) | (1 << UCSZ0)| (1 << UCSZ1);
}

void send_string(const char* word){
    int size = strlen(word);
    for(int i = 0; i < size ; i++){
        while(!(UCSRA & (1 << UDRE)));
        UDR = word[i];
    }
}

int main(void)
{
    int i = 0;
    init_uart();

    DDRB |= _BV(0);
    PORTB |= _BV(0);
    while (1)
    {
        // PORTB ^= _BV(0);
        // while(!(UCSRA & (1 << UDRE))){};
        // UDR = i++ % 2 ?  'A' : 'B';
        send_string("siema\r\n");
        _delay_ms(200);
    };
    
}