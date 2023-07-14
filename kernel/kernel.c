void main()
{
  // Create a pointer to a char and point it to a text cell in video memory.
  char *character = (char *) 0xb8000 + 2400;
  *character = 'E';
}