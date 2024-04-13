read_and_display_attr (unsigned long           attribute,
		       unsigned long           form,
		       dwarf_signed_vma        implicit_const,
		       unsigned char *         start,
		       unsigned char *         data,
		       unsigned char *         end,
		       dwarf_vma               cu_offset,
		       dwarf_vma               pointer_size,
		       dwarf_vma               offset_size,
		       int                     dwarf_version,
		       debug_info *            debug_info_p,
		       int                     do_loc,
		       struct dwarf_section *  section,
		       struct cu_tu_set *      this_set,
		       int                     level)
{
  if (!do_loc)
    printf ("   %-18s:", get_AT_name (attribute));
  data = read_and_display_attr_value (attribute, form, implicit_const,
				      start, data, end,
				      cu_offset, pointer_size, offset_size,
				      dwarf_version, debug_info_p,
				      do_loc, section, this_set, ' ', level);
  if (!do_loc)
    printf ("\n");
  return data;
}