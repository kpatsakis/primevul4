read_leb128 (unsigned char *data,
	     const unsigned char *const end,
	     bool sign,
	     unsigned int *length_return,
	     int *status_return)
{
  dwarf_vma result = 0;
  unsigned int num_read = 0;
  unsigned int shift = 0;
  int status = 1;

  while (data < end)
    {
      unsigned char byte = *data++;
      unsigned char lost, mask;

      num_read++;

      if (shift < CHAR_BIT * sizeof (result))
	{
	  result |= ((dwarf_vma) (byte & 0x7f)) << shift;
	  /* These bits overflowed.  */
	  lost = byte ^ (result >> shift);
	  /* And this is the mask of possible overflow bits.  */
	  mask = 0x7f ^ ((dwarf_vma) 0x7f << shift >> shift);
	  shift += 7;
	}
      else
	{
	  lost = byte;
	  mask = 0x7f;
	}
      if ((lost & mask) != (sign && (dwarf_signed_vma) result < 0 ? mask : 0))
	status |= 2;

      if ((byte & 0x80) == 0)
	{
	  status &= ~1;
	  if (sign && shift < CHAR_BIT * sizeof (result) && (byte & 0x40))
	    result |= -((dwarf_vma) 1 << shift);
	  break;
	}
    }

  if (length_return != NULL)
    *length_return = num_read;
  if (status_return != NULL)
    *status_return = status;

  return result;
}