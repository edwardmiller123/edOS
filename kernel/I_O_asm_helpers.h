#ifndef I_O_ASM_HELPERS
#define I_O_ASM_HELPERS
// Wrapper functions for accesing the I/O addresses or decice controllers.
// word = 2bytes (16-bits)
// "a" refers to register eax and "d" edx

// Assembly wrapper that reads a byte from the specified I/O address (port).
unsigned char port_byte_in(unsigned short port)
{
  // port address is written to dx. The contents stored there is then
  // stored in al which we return as result. This can then be manipulated.

  /* equivalent assembly:
  mov dx, port
  in al, dx
  */

  // gcc inline assembly uses reversed syntax order to nasm i.e
  // mov src, dest
  unsigned char result;
  __asm__("in %%dx, %%al"
          : "=a"(result)
          : "d"(port));
  return result;
}

// Assembly wrapper that writes a byte to the specifed I/O address.
void port_byte_out(unsigned short port, unsigned char data)
{
  // set the port address we want to access to dx and the data we want to store in that port address
  // in al. We then store the contense of al in dx inorder to write the data to
  // that I/O address.
  __asm__("out %%al, %%dx"
          :
          : "a"(data), "d"(port));
}

// sendCommand is a wrapper around the asm helpers that writes a command byte to a specified
// port and reads the response from the specified port.
int sendCommand(unsigned char commandToSend, unsigned short readPort, unsigned short writePort)
{
  port_byte_out(writePort, commandToSend);
  int result = port_byte_in(readPort);
  return result;
}

// sendCommandWithData sends a command with a data byte to the specified port and returns the response the from
// specified read port.
int sendCommandWithData(unsigned char commandToSend, unsigned char dataToSend, unsigned short readPort, unsigned short writePort)
{
  port_byte_out(writePort, commandToSend);
  port_byte_out(writePort, dataToSend);
  int result = port_byte_in(readPort);
  return result;
}

// Assembly wrapper that reads two bytes from the specified I/O address (port).
unsigned short port_word_in(unsigned short port)
{
  unsigned short result;
  __asm__("in %%dx, %%ax"
          : "=a"(result)
          : "d"(port));
  return result;
}

// Assembly wrapper that writes two bytes to the specified I/O address
void port_word_out(unsigned short port, unsigned char data)
{
  __asm__("out %%ax, %%dx"
          :
          : "a"(data), "d"(port));
}

#endif
