fetch_indexed_addr (dwarf_vma offset, uint32_t num_bytes)
{
  struct dwarf_section *section = &debug_displays [debug_addr].section;

  if (section->start == NULL)
    {
      warn (_("<no .debug_addr section>"));
      return 0;
    }

  if (offset + num_bytes > section->size)
    {
      warn (_("Offset into section %s too big: 0x%s\n"),
	    section->name, dwarf_vmatoa ("x", offset));
      return 0;
    }

  return byte_get (section->start + offset, num_bytes);
}