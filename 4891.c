get_encoded_value (unsigned char **pdata,
		   int encoding,
		   struct dwarf_section *section,
		   unsigned char * end)
{
  unsigned char * data = * pdata;
  unsigned int size = size_of_encoded_value (encoding);
  dwarf_vma val;

  if (data >= end || size > (size_t) (end - data))
    {
      warn (_("Encoded value extends past end of section\n"));
      * pdata = end;
      return 0;
    }

  /* PR 17512: file: 002-829853-0.004.  */
  if (size > 8)
    {
      warn (_("Encoded size of %d is too large to read\n"), size);
      * pdata = end;
      return 0;
    }

  /* PR 17512: file: 1085-5603-0.004.  */
  if (size == 0)
    {
      warn (_("Encoded size of 0 is too small to read\n"));
      * pdata = end;
      return 0;
    }

  if (encoding & DW_EH_PE_signed)
    val = byte_get_signed (data, size);
  else
    val = byte_get (data, size);

  if ((encoding & 0x70) == DW_EH_PE_pcrel)
    val += section->address + (data - section->start);

  * pdata = data + size;
  return val;
}