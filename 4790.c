fetch_indexed_value (dwarf_vma idx,
		     enum dwarf_section_display_enum sec_enum,
		     dwarf_vma base_address)
{
  struct dwarf_section *section = &debug_displays [sec_enum].section;

  if (section->start == NULL)
    {
      warn (_("Unable to locate %s section\n"), section->uncompressed_name);
      return 0;
    }

  uint32_t pointer_size, bias;

  if (byte_get (section->start, 4) == 0xffffffff)
    {
      pointer_size = 8;
      bias = 20;
    }
  else
    {
      pointer_size = 4;
      bias = 12;
    }
 
  dwarf_vma offset = idx * pointer_size;

  /* Offsets are biased by the size of the section header
     or base address.  */
  if (sec_enum == loclists)
    offset += base_address;
  else
    offset += bias;

  if (offset + pointer_size > section->size)
    {
      warn (_("Offset into section %s too big: 0x%s\n"),
	    section->name, dwarf_vmatoa ("x", offset));
      return 0;
    }

  return byte_get (section->start + offset, pointer_size);
}