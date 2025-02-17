combineSeparateSamples16bits (uint8 *in[], uint8 *out, uint32 cols,
                              uint32 rows, uint16 spp, uint16 bps, 
 	                      FILE *dumpfile, int format, int level)
  {
  int    ready_bits = 0 /*, bytes_per_sample = 0 */;
  uint32 src_rowsize, dst_rowsize; 
  uint32 bit_offset, src_offset;
  uint32 row, col, src_byte = 0, src_bit = 0;
  uint16 maskbits = 0, matchbits = 0;
  uint16 buff1 = 0, buff2 = 0;
  uint8  bytebuff = 0;
  tsample_t s;
  unsigned char *src = in[0];
  unsigned char *dst = out;
  char           action[8];

  if ((src == NULL) || (dst == NULL))
    {
    TIFFError("combineSeparateSamples16bits","Invalid input or output buffer");
    return (1);
    }

  /* bytes_per_sample = (bps + 7) / 8; */ 
  src_rowsize = ((bps * cols) + 7) / 8;
  dst_rowsize = ((bps * cols * spp) + 7) / 8;
  maskbits = (uint16)-1 >> (16 - bps);

  for (row = 0; row < rows; row++)
    {
    ready_bits = 0;
    buff1 = buff2 = 0;
    dst = out + (row * dst_rowsize);
    src_offset = row * src_rowsize;
    for (col = 0; col < cols; col++)
      {
      /* Compute src byte(s) and bits within byte(s) */
      bit_offset = col * bps;
      src_byte = bit_offset / 8;
      src_bit  = bit_offset % 8;

      matchbits = maskbits << (16 - src_bit - bps); 
      for (s = 0; (s < spp) && (s < MAX_SAMPLES); s++)
        {
	src = in[s] + src_offset + src_byte;
        if (little_endian)
          buff1 = (src[0] << 8) | src[1];
        else
          buff1 = (src[1] << 8) | src[0];

	buff1 = (buff1 & matchbits) << (src_bit);

	/* If we have a full buffer's worth, write it out */
	if (ready_bits >= 8)
	  {
	    bytebuff = (buff2 >> 8);
	    *dst++ = bytebuff;
	    ready_bits -= 8;
	    /* shift in new bits */
	    buff2 = ((buff2 << 8) | (buff1 >> ready_bits));
	    strcpy (action, "Flush");
	  }
	else
	  { /* add another bps bits to the buffer */
	    bytebuff = 0;
	    buff2 = (buff2 | (buff1 >> ready_bits));
	    strcpy (action, "Update");
	  }
	ready_bits += bps;

	if ((dumpfile != NULL) && (level == 3))
	  {
	  dump_info (dumpfile, format, "",
		       "Row %3d, Col %3d, Samples %d, Src byte offset %3d  bit offset %2d  Dst offset %3d",
		       row + 1, col + 1, s, src_byte, src_bit, dst - out);

	  dump_short (dumpfile, format, "Match bits", matchbits);
	  dump_data  (dumpfile, format, "Src   bits", src, 2);
	  dump_short (dumpfile, format, "Buff1 bits", buff1);
	  dump_short (dumpfile, format, "Buff2 bits", buff2);
	  dump_byte  (dumpfile, format, "Write byte", bytebuff);
	  dump_info  (dumpfile, format, "","Ready bits:  %d, %s", ready_bits, action); 
	  }
	}
      }

    /* catch any trailing bits at the end of the line */
    if (ready_bits > 0)
      {
      bytebuff = (buff2 >> 8);
      *dst++ = bytebuff;
      if ((dumpfile != NULL) && (level == 3))
	{
	dump_info (dumpfile, format, "",
		       "Row %3d, Col %3d, Src byte offset %3d  bit offset %2d  Dst offset %3d",
		       row + 1, col + 1, src_byte, src_bit, dst - out);
	dump_byte (dumpfile, format, "Final bits", bytebuff);
	}
      }

    if ((dumpfile != NULL) && (level == 2))
      {
      dump_info (dumpfile, format, "combineSeparateSamples16bits","Output data");
      dump_buffer(dumpfile, format, 1, dst_rowsize, row, out + (row * dst_rowsize));
      }
    }

  return (0);
  } /* end combineSeparateSamples16bits */
