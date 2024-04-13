static void WriteProfileShort(const EndianType endian,
  const unsigned short value,unsigned char *p)
{
  unsigned char
    buffer[2];

  if (endian == LSBEndian)
    {
      buffer[0]=(unsigned char) value;
      buffer[1]=(unsigned char) (value >> 8);
      (void) CopyMagickMemory(p,buffer,2);
      return;
    }
  buffer[0]=(unsigned char) (value >> 8);
  buffer[1]=(unsigned char) value;
  (void) CopyMagickMemory(p,buffer,2);
}
