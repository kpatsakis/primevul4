display_view_pair_list (struct dwarf_section *section,
			unsigned char **vstart_ptr,
			unsigned int debug_info_entry,
			unsigned char *vlistend)
{
  unsigned char *vstart = *vstart_ptr;
  unsigned char *section_end = section->start + section->size;
  unsigned int pointer_size = debug_information [debug_info_entry].pointer_size;

  if (vlistend < section_end)
    section_end = vlistend;

  putchar ('\n');

  while (vstart < section_end)
    {
      dwarf_vma off = vstart - section->start;
      dwarf_vma vbegin, vend;

      READ_ULEB (vbegin, vstart, section_end);
      if (vstart == section_end)
	break;

      READ_ULEB (vend, vstart, section_end);
      printf ("    %8.8lx ", (unsigned long) off);

      print_dwarf_view (vbegin, pointer_size, 1);
      print_dwarf_view (vend, pointer_size, 1);
      printf (_("location view pair\n"));
    }

  putchar ('\n');
  *vstart_ptr = vstart;
}