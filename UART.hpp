#ifndef UART_HPP
#define UART_HPP


void sendMotorControllerSpeedByte(long LeftControllerInput, long RightControllerInput);
void initUart(void);
void putCharRaspberryPi2(int *p_tx_buffer);


#endif /* UART_HPP */