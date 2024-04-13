static inline size_t GetPSDPacketSize(Image *image)
{
  if (image->storage_class == PseudoClass)
    {
      if (image->colors > 256)
        return(2);
      else if (image->depth > 8)
        return(2);
    }
  else
    if (image->depth > 8)
      return(2);

  return(1);
}
