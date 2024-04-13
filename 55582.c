static size_t WritePascalString(Image *image,const char *value,size_t padding)
{
  size_t
    count,
    length;

  register ssize_t
    i;

  /*
    Max length is 255.
  */
  count=0;
  length=(strlen(value) > 255UL ) ? 255UL : strlen(value);
  if (length ==  0)
    count+=WriteBlobByte(image,0);
  else
    {
      count+=WriteBlobByte(image,(unsigned char) length);
      count+=WriteBlob(image,length,(const unsigned char *) value);
    }
  length++;
  if ((length % padding) == 0)
    return(count);
  for (i=0; i < (ssize_t) (padding-(length % padding)); i++)
    count+=WriteBlobByte(image,0);
  return(count);
}
