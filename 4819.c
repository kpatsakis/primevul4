display_debug_abbrev (struct dwarf_section *section,
		      void *file ATTRIBUTE_UNUSED)
{
  abbrev_entry *entry;
  unsigned char *start = section->start;

  introduce (section, false);

  do
    {
      abbrev_list *    list;
      dwarf_vma        offset;

      offset = start - section->start;
      list = find_abbrev_list_by_abbrev_offset (0, offset);
      if (list == NULL)
	{
	  list = new_abbrev_list (0, offset);
	  start = process_abbrev_set (section, 0, section->size, offset, list);
	  list->start_of_next_abbrevs = start;
	}
      else
	start = list->start_of_next_abbrevs;

      if (list->first_abbrev == NULL)
	continue;

      printf (_("  Number TAG (0x%lx)\n"), (long) offset);

      for (entry = list->first_abbrev; entry; entry = entry->next)
	{
	  abbrev_attr *attr;

	  printf ("   %ld      %s    [%s]\n",
		  entry->number,
		  get_TAG_name (entry->tag),
		  entry->children ? _("has children") : _("no children"));

	  for (attr = entry->first_attr; attr; attr = attr->next)
	    {
	      printf ("    %-18s %s",
		      get_AT_name (attr->attribute),
		      get_FORM_name (attr->form));
	      if (attr->form == DW_FORM_implicit_const)
		printf (": %s", dwarf_vmatoa ("d", attr->implicit_const));
	      putchar ('\n');
	    }
	}
    }
  while (start);

  printf ("\n");

  return 1;
}