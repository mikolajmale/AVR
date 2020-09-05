#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define UART_BAUD       4800  
#define __UBRR          (( F_CPU + UART_BAUD*8UL) / (16UL*UART_BAUD)-1)  

#define BUFFER_SIZE     20

struct buffer{
    char buffer[BUFFER_SIZE];
    uint8_t i = 0;
}RX_BUFFER, TX_BUFFER;

void clear_buffer(struct buffer buf){
    buf.i = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) buf.buffer[i] = 0; 
} 

char save_byte(const char byte){
    if (RX_BUFFER.i == BUFFER_SIZE) clear_buffer(RX_BUFFER);
    RX_BUFFER.buffer[RX_BUFFER.i++] = byte;
    return RX_BUFFER.buffer[RX_BUFFER.i-1];
}

void init_uart(){
    DDRD |= _BV(1); // TX
    DDRD &= _BV(0); // RX

    UBRRH = (unsigned char)(__UBRR >> 8);
    UBRRL = (unsigned char)(__UBRR);

    UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE)| (1 << TXCIE);
    UCSRC |= (1 << URSEL) | (1 << UCSZ0)| (1 << UCSZ1);
    clear_buffer(RX_BUFFER);
    clear_buffer(TX_BUFFER);
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
    init_uart();
    sei();
    DDRB |= _BV(0);
    while (1)
    {
        
        PORTB ^= _BV(0);
        send_string("hello world\r\n");
        _delay_ms(1000);
    };
    
}

ISR(USART_RXC_vect){
    char sign = UDR;
    if (sign == 27){ //esc
        send_string(RX_BUFFER.buffer);
        clear_buffer(RX_BUFFER);
    }
    else save_byte(sign);
}

ISR(USART_TXC_vect){


}

