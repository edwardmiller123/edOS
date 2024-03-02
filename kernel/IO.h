#ifndef IO
#define IO
// Wrapper functions for accesing the I/O addresses or decice controllers.
// word = 2bytes (16-bits)
// "a" refers to register eax and "d" edx

// Assembly wrapper that reads a byte from the specified I/O address (port).
unsigned char port_byte_in(unsigned short port);

// Assembly wrapper that writes a byte to the specifed I/O address.
void port_byte_out(unsigned short port, unsigned char data);

// sendCommand is a wrapper around the asm helpers that writes a command byte to a specified
// port and reads the response from the specified port.
int sendCommand(unsigned char commandToSend, unsigned short readPort, unsigned short writePort);

// sendCommandWithData sends a command with a data byte to the specified port and returns the response the from
// specified read port.
int sendCommandWithData(unsigned char commandToSend, unsigned char dataToSend, unsigned short readPort, unsigned short writePort);

// Assembly wrapper that reads two bytes from the specified I/O address (port).
unsigned short port_word_in(unsigned short port);

// Assembly wrapper that writes two bytes to the specified I/O address
void port_word_out(unsigned short port, unsigned char data);

#endif
