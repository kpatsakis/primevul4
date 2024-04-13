display_debug_macinfo (struct dwarf_section *section,
		       void *file ATTRIBUTE_UNUSED)
{
  unsigned char *start = section->start;
  unsigned char *end = start + section->size;
  unsigned char *curr = start;
  enum dwarf_macinfo_record_type op;

  introduce (section, false);

  while (curr < end)
    {
      unsigned int lineno;
      const unsigned char *string;

      op = (enum dwarf_macinfo_record_type) *curr;
      curr++;

      switch (op)
	{
	case DW_MACINFO_start_file:
	  {
	    unsigned int filenum;

	    READ_ULEB (lineno, curr, end);
	    READ_ULEB (filenum, curr, end);
	    printf (_(" DW_MACINFO_start_file - lineno: %d filenum: %d\n"),
		    lineno, filenum);
	  }
	  break;

	case DW_MACINFO_end_file:
	  printf (_(" DW_MACINFO_end_file\n"));
	  break;

	case DW_MACINFO_define:
	  READ_ULEB (lineno, curr, end);
	  string = curr;
	  curr += strnlen ((char *) string, end - string);
	  printf (_(" DW_MACINFO_define - lineno : %d macro : %*s\n"),
		  lineno, (int) (curr - string), string);
	  if (curr < end)
	    curr++;
	  break;

	case DW_MACINFO_undef:
	  READ_ULEB (lineno, curr, end);
	  string = curr;
	  curr += strnlen ((char *) string, end - string);
	  printf (_(" DW_MACINFO_undef - lineno : %d macro : %*s\n"),
		  lineno, (int) (curr - string), string);
	  if (curr < end)
	    curr++;
	  break;

	case DW_MACINFO_vendor_ext:
	  {
	    unsigned int constant;

	    READ_ULEB (constant, curr, end);
	    string = curr;
	    curr += strnlen ((char *) string, end - string);
	    printf (_(" DW_MACINFO_vendor_ext - constant : %d string : %*s\n"),
		    constant, (int) (curr - string), string);
	    if (curr < end)
	      curr++;
	  }
	  break;
	}
    }

  return 1;
}