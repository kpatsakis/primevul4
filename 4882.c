fetch_indexed_string (dwarf_vma           idx,
		      struct cu_tu_set *  this_set,
		      dwarf_vma           offset_size,
		      bool                dwo,
		      dwarf_vma           str_offsets_base)
{
  enum dwarf_section_display_enum str_sec_idx = dwo ? str_dwo : str;
  enum dwarf_section_display_enum idx_sec_idx = dwo ? str_index_dwo : str_index;
  struct dwarf_section *index_section = &debug_displays [idx_sec_idx].section;
  struct dwarf_section *str_section = &debug_displays [str_sec_idx].section;
  dwarf_vma index_offset;
  dwarf_vma str_offset;
  const char * ret;

  if (index_section->start == NULL)
    return (dwo ? _("<no .debug_str_offsets.dwo section>")
		: _("<no .debug_str_offsets section>"));

  if (str_section->start == NULL)
    return (dwo ? _("<no .debug_str.dwo section>")
		: _("<no .debug_str section>"));

  index_offset = idx * offset_size;

  if (this_set != NULL)
    index_offset += this_set->section_offsets [DW_SECT_STR_OFFSETS];

  index_offset += str_offsets_base;

  if (index_offset + offset_size > index_section->size)
    {
      warn (_("string index of %s converts to an offset of 0x%s which is too big for section %s"),
	    dwarf_vmatoa ("d", idx),
	    dwarf_vmatoa ("x", index_offset),
	    str_section->name);

      return _("<string index too big>");
    }

  /* FIXME: If we are being paranoid then we should also check to see if
     IDX references an entry beyond the end of the string table pointed to
     by STR_OFFSETS_BASE.  (Since there can be more than one string table
     in a DWARF string section).  */

  str_offset = byte_get (index_section->start + index_offset, offset_size);

  str_offset -= str_section->address;
  if (str_offset >= str_section->size)
    {
      warn (_("indirect offset too big: 0x%s\n"),
	    dwarf_vmatoa ("x", str_offset));
      return _("<indirect index offset is too big>");
    }

  ret = (const char *) str_section->start + str_offset;

  /* Unfortunately we cannot rely upon str_section ending with a NUL byte.
     Since our caller is expecting to receive a well formed C string we test
     for the lack of a terminating byte here.  */
  if (strnlen (ret, str_section->size - str_offset)
      == str_section->size - str_offset)
    return _("<no NUL byte at end of section>");

  return ret;
}