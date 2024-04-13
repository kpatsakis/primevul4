display_formatted_table (unsigned char *data,
			 unsigned char *start,
			 unsigned char *end,
			 const DWARF2_Internal_LineInfo *linfo,
			 struct dwarf_section *section,
			 bool is_dir)
{
  unsigned char *format_start, format_count, *format, formati;
  dwarf_vma data_count, datai;
  unsigned int namepass, last_entry = 0;
  const char * table_name = is_dir ? N_("Directory Table") : N_("File Name Table");

  SAFE_BYTE_GET_AND_INC (format_count, data, 1, end);
  if (do_checks && format_count > 5)
    warn (_("Unexpectedly large number of columns in the %s (%u)\n"),
	  table_name, format_count);

  format_start = data;
  for (formati = 0; formati < format_count; formati++)
    {
      SKIP_ULEB (data, end);
      SKIP_ULEB (data, end);
      if (data >= end)
	{
	  warn (_("%s: Corrupt format description entry\n"), table_name);
	  return data;
	}
    }

  READ_ULEB (data_count, data, end);
  if (data_count == 0)
    {
      printf (_("\n The %s is empty.\n"), table_name);
      return data;
    }
  else if (data >= end)
    {
      warn (_("%s: Corrupt entry count - expected %s but none found\n"),
	    table_name, dwarf_vmatoa ("x", data_count));
      return data;
    }

  else if (format_count == 0)
    {
      warn (_("%s: format count is zero, but the table is not empty\n"),
	    table_name);
      return end;
    }

  printf (_("\n The %s (offset 0x%lx, lines %s, columns %u):\n"),
	  table_name, (long) (data - start), dwarf_vmatoa ("u", data_count),
	  format_count);

  printf (_("  Entry"));
  /* Delay displaying name as the last entry for better screen layout.  */
  for (namepass = 0; namepass < 2; namepass++)
    {
      format = format_start;
      for (formati = 0; formati < format_count; formati++)
	{
	  dwarf_vma content_type;

	  READ_ULEB (content_type, format, end);
	  if ((content_type == DW_LNCT_path) == (namepass == 1))
	    switch (content_type)
	      {
	      case DW_LNCT_path:
		printf (_("\tName"));
		break;
	      case DW_LNCT_directory_index:
		printf (_("\tDir"));
		break;
	      case DW_LNCT_timestamp:
		printf (_("\tTime"));
		break;
	      case DW_LNCT_size:
		printf (_("\tSize"));
		break;
	      case DW_LNCT_MD5:
		printf (_("\tMD5\t\t\t"));
		break;
	      default:
		printf (_("\t(Unknown format content type %s)"),
			dwarf_vmatoa ("u", content_type));
	      }
	  SKIP_ULEB (format, end);
	}
    }
  putchar ('\n');

  for (datai = 0; datai < data_count; datai++)
    {
      unsigned char *datapass = data;

      printf ("  %d", last_entry++);
      /* Delay displaying name as the last entry for better screen layout.  */
      for (namepass = 0; namepass < 2; namepass++)
	{
	  format = format_start;
	  data = datapass;
	  for (formati = 0; formati < format_count; formati++)
	    {
	      dwarf_vma content_type, form;

	      READ_ULEB (content_type, format, end);
	      READ_ULEB (form, format, end);
	      data = read_and_display_attr_value (0, form, 0, start, data, end,
						  0, 0, linfo->li_offset_size,
						  linfo->li_version, NULL,
			    ((content_type == DW_LNCT_path) != (namepass == 1)),
						  section, NULL, '\t', -1);
	    }
	}

      if (data >= end && (datai < data_count - 1))
	{
	  warn (_("\n%s: Corrupt entries list\n"), table_name);
	  return data;
	}
      putchar ('\n');
    }
  return data;
}