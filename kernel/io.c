// Wrapper functions for accesing the IO addresses or device controllers.
// word = 2bytes (16-bits)
// "a" refers to register eax and "d" edx

// Assembly wrapper that reads a byte from the specified IO address (port).
unsigned char readByte(unsigned short port)
{
  // port address is written to dx. The contents stored there is then
  // stored in al which we return as result. This can then be manipulated.

  /* equivalent NASM syntax assembly:
  mov dx, port
  in al, dx
  */

  // gcc inline assembly uses reversed syntax order to nasm i.e
  // mov src, dest
  unsigned char result;
  __asm__ volatile("in %%dx, %%al"
                   : "=a"(result)
                   : "d"(port));
  return result;
}

// Assembly wrapper that writes a byte to the specifed IO address.
void writeByte(unsigned short port, unsigned char data)
{
  // set the port address we want to access to dx and the data we want to store in that port address
  // in al. We then store the contense of al in dx inorder to write the data to
  // that IO address.
  __asm__ volatile("out %%al, %%dx"
                   :
                   : "a"(data), "d"(port));
}

// sendCommand is a wrapper around the asm helpers that writes a command byte to a specified
// port and reads the response from the specified port.
int sendCommand(unsigned char commandToSend, unsigned short readPort, unsigned short writePort)
{
  writeByte(writePort, commandToSend);
  int result = readByte(readPort);
  return result;
}

// sendCommandWithData sends a command with a data byte to the specified port and returns the response the from
// specified read port.
int sendCommandWithData(unsigned char commandToSend, unsigned char dataToSend, unsigned short readPort, unsigned short writePort)
{
  writeByte(writePort, commandToSend);
  writeByte(writePort, dataToSend);
  int result = readByte(readPort);
  return result;
}

// Assembly wrapper that reads two bytes from the specified IO address (port).
unsigned short port_word_in(unsigned short port)
{
  unsigned short result;
  __asm__("in %%dx, %%ax"
          : "=a"(result)
          : "d"(port));
  return result;
}

// Assembly wrapper that writes two bytes to the specified IO address
void port_word_out(unsigned short port, unsigned char data)
{
  __asm__("out %%ax, %%dx"
          :
          : "a"(data), "d"(port));
}