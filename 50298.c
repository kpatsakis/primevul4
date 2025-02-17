reverseSamplesBytes (uint16 spp, uint16 bps, uint32 width, 
                     uint8 *src, uint8 *dst)
  {
  int i;
  uint32  col, bytes_per_pixel, col_offset;
  uint8   bytebuff1;
  unsigned char swapbuff[32];
  
  if ((src == NULL) || (dst == NULL))
    {
    TIFFError("reverseSamplesBytes","Invalid input or output buffer");
    return (1);
    }

  bytes_per_pixel  = ((bps * spp) + 7) / 8;
  if( bytes_per_pixel > sizeof(swapbuff) )
  {
    TIFFError("reverseSamplesBytes","bytes_per_pixel too large");
    return (1);
  }
  switch (bps / 8)
     {
     case 8:  /* Use memcpy for multiple bytes per sample data */
     case 4:
     case 3:
     case 2: for (col = 0; col < (width / 2); col++)
               {
	       col_offset = col * bytes_per_pixel;                     
	       _TIFFmemcpy (swapbuff, src + col_offset, bytes_per_pixel);
	       _TIFFmemcpy (src + col_offset, dst - col_offset - bytes_per_pixel, bytes_per_pixel);
	       _TIFFmemcpy (dst - col_offset - bytes_per_pixel, swapbuff, bytes_per_pixel);
               }
	     break;
     case 1: /* Use byte copy only for single byte per sample data */
             for (col = 0; col < (width / 2); col++)
               { 
	       for (i = 0; i < spp; i++)
                  {
		  bytebuff1 = *src;
		  *src++ = *(dst - spp + i);
                  *(dst - spp + i) = bytebuff1;
		  }
		dst -= spp;
                }
	     break;
     default: TIFFError("reverseSamplesBytes","Unsupported bit depth %d", bps);
       return (1);
     }
  return (0);
  } /* end reverseSamplesBytes */
