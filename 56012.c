static void ReadBlobFloatsLSB(Image * image, size_t len, float *data)
{
  while (len >= 4)
  {
    *data++ = ReadBlobFloat(image);
    len -= sizeof(float);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}
