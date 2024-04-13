display_debug_macro (struct dwarf_section *section,
		     void *file)
{
  unsigned char *start = section->start;
  unsigned char *end = start + section->size;
  unsigned char *curr = start;
  unsigned char *extended_op_buf[256];
  bool is_dwo = false;
  const char *suffix = strrchr (section->name, '.');

  if (suffix && strcmp (suffix, ".dwo") == 0)
    is_dwo = true;

  load_debug_section_with_follow (str, file);
  load_debug_section_with_follow (line, file);
  load_debug_section_with_follow (str_index, file);
  
  introduce (section, false);

  while (curr < end)
    {
      unsigned int lineno, version, flags;
      unsigned int offset_size;
      const unsigned char *string;
      dwarf_vma line_offset = 0, sec_offset = curr - start, offset;
      unsigned char **extended_ops = NULL;

      SAFE_BYTE_GET_AND_INC (version, curr, 2, end);
      if (version != 4 && version != 5)
	{
	  error (_("Expected to find a version number of 4 or 5 in section %s but found %d instead\n"),
		 section->name, version);
	  return 0;
	}

      SAFE_BYTE_GET_AND_INC (flags, curr, 1, end);
      offset_size = (flags & 1) ? 8 : 4;
      printf (_("  Offset:                      0x%lx\n"),
	      (unsigned long) sec_offset);
      printf (_("  Version:                     %d\n"), version);
      printf (_("  Offset size:                 %d\n"), offset_size);
      if (flags & 2)
	{
	  SAFE_BYTE_GET_AND_INC (line_offset, curr, offset_size, end);
	  printf (_("  Offset into .debug_line:     0x%lx\n"),
		  (unsigned long) line_offset);
	}
      if (flags & 4)
	{
	  unsigned int i, count, op;
	  dwarf_vma nargs, n;

	  SAFE_BYTE_GET_AND_INC (count, curr, 1, end);

	  memset (extended_op_buf, 0, sizeof (extended_op_buf));
	  extended_ops = extended_op_buf;
	  if (count)
	    {
	      printf (_("  Extension opcode arguments:\n"));
	      for (i = 0; i < count; i++)
		{
		  SAFE_BYTE_GET_AND_INC (op, curr, 1, end);
		  extended_ops[op] = curr;
		  READ_ULEB (nargs, curr, end);
		  if (nargs == 0)
		    printf (_("    DW_MACRO_%02x has no arguments\n"), op);
		  else
		    {
		      printf (_("    DW_MACRO_%02x arguments: "), op);
		      for (n = 0; n < nargs; n++)
			{
			  unsigned int form;

			  SAFE_BYTE_GET_AND_INC (form, curr, 1, end);
			  printf ("%s%s", get_FORM_name (form),
				  n == nargs - 1 ? "\n" : ", ");
			  switch (form)
			    {
			    case DW_FORM_data1:
			    case DW_FORM_data2:
			    case DW_FORM_data4:
			    case DW_FORM_data8:
			    case DW_FORM_sdata:
			    case DW_FORM_udata:
			    case DW_FORM_block:
			    case DW_FORM_block1:
			    case DW_FORM_block2:
			    case DW_FORM_block4:
			    case DW_FORM_flag:
			    case DW_FORM_string:
			    case DW_FORM_strp:
			    case DW_FORM_sec_offset:
			      break;
			    default:
			      error (_("Invalid extension opcode form %s\n"),
				     get_FORM_name (form));
			      return 0;
			    }
			}
		    }
		}
	    }
	}
      printf ("\n");

      while (1)
	{
	  unsigned int op;

	  if (curr >= end)
	    {
	      error (_(".debug_macro section not zero terminated\n"));
	      return 0;
	    }

	  SAFE_BYTE_GET_AND_INC (op, curr, 1, end);
	  if (op == 0)
	    break;

	  switch (op)
	    {
	    case DW_MACRO_define:
	      READ_ULEB (lineno, curr, end);
	      string = curr;
	      curr += strnlen ((char *) string, end - string);
	      printf (_(" DW_MACRO_define - lineno : %d macro : %*s\n"),
		      lineno, (int) (curr - string), string);
	      if (curr < end)
		curr++;
	      break;

	    case DW_MACRO_undef:
	      READ_ULEB (lineno, curr, end);
	      string = curr;
	      curr += strnlen ((char *) string, end - string);
	      printf (_(" DW_MACRO_undef - lineno : %d macro : %*s\n"),
		      lineno, (int) (curr - string), string);
	      if (curr < end)
		curr++;
	      break;

	    case DW_MACRO_start_file:
	      {
		unsigned int filenum;
		unsigned char *file_name = NULL, *dir_name = NULL;

		READ_ULEB (lineno, curr, end);
		READ_ULEB (filenum, curr, end);

		if ((flags & 2) == 0)
		  error (_("DW_MACRO_start_file used, but no .debug_line offset provided.\n"));
		else
		  file_name
		    = get_line_filename_and_dirname (line_offset, filenum,
						     &dir_name);
		if (file_name == NULL)
		  printf (_(" DW_MACRO_start_file - lineno: %d filenum: %d\n"),
			  lineno, filenum);
		else
		  printf (_(" DW_MACRO_start_file - lineno: %d filenum: %d filename: %s%s%s\n"),
			  lineno, filenum,
			  dir_name != NULL ? (const char *) dir_name : "",
			  dir_name != NULL ? "/" : "", file_name);
	      }
	      break;

	    case DW_MACRO_end_file:
	      printf (_(" DW_MACRO_end_file\n"));
	      break;

	    case DW_MACRO_define_strp:
	      READ_ULEB (lineno, curr, end);
	      if (version == 4 && is_dwo)
		READ_ULEB (offset, curr, end);
	      else
		SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      string = fetch_indirect_string (offset);
	      printf (_(" DW_MACRO_define_strp - lineno : %d macro : %s\n"),
		      lineno, string);
	      break;

	    case DW_MACRO_undef_strp:
	      READ_ULEB (lineno, curr, end);
	      if (version == 4 && is_dwo)
		READ_ULEB (offset, curr, end);
	      else
		SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      string = fetch_indirect_string (offset);
	      printf (_(" DW_MACRO_undef_strp - lineno : %d macro : %s\n"),
		      lineno, string);
	      break;

	    case DW_MACRO_import:
	      SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      printf (_(" DW_MACRO_import - offset : 0x%lx\n"),
		      (unsigned long) offset);
	      break;

	    case DW_MACRO_define_sup:
	      READ_ULEB (lineno, curr, end);
	      SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      printf (_(" DW_MACRO_define_sup - lineno : %d macro offset : 0x%lx\n"),
		      lineno, (unsigned long) offset);
	      break;

	    case DW_MACRO_undef_sup:
	      READ_ULEB (lineno, curr, end);
	      SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      printf (_(" DW_MACRO_undef_sup - lineno : %d macro offset : 0x%lx\n"),
		      lineno, (unsigned long) offset);
	      break;

	    case DW_MACRO_import_sup:
	      SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
	      printf (_(" DW_MACRO_import_sup - offset : 0x%lx\n"),
		      (unsigned long) offset);
	      break;

	    case DW_MACRO_define_strx:
	    case DW_MACRO_undef_strx:
	      READ_ULEB (lineno, curr, end);
	      READ_ULEB (offset, curr, end);
	      string = (const unsigned char *)
		fetch_indexed_string (offset, NULL, offset_size, false, 0);
	      if (op == DW_MACRO_define_strx)
		printf (" DW_MACRO_define_strx ");
	      else
		printf (" DW_MACRO_undef_strx ");
	      if (do_wide)
		printf (_("(with offset %s) "), dwarf_vmatoa ("x", offset));
	      printf (_("lineno : %d macro : %s\n"),
		      lineno, string);
	      break;

	    default:
	      if (op >= DW_MACRO_lo_user && op <= DW_MACRO_hi_user)
		{
		  printf (_(" <Target Specific macro op: %#x - UNHANDLED"), op);
		  break;
		}

	      if (extended_ops == NULL || extended_ops[op] == NULL)
		{
		  error (_(" Unknown macro opcode %02x seen\n"), op);
		  return 0;
		}
	      else
		{
		  /* Skip over unhandled opcodes.  */
		  dwarf_vma nargs, n;
		  unsigned char *desc = extended_ops[op];
		  READ_ULEB (nargs, desc, end);
		  if (nargs == 0)
		    {
		      printf (_(" DW_MACRO_%02x\n"), op);
		      break;
		    }
		  printf (_(" DW_MACRO_%02x -"), op);
		  for (n = 0; n < nargs; n++)
		    {
		      int val;

		      /* DW_FORM_implicit_const is not expected here.  */
		      SAFE_BYTE_GET_AND_INC (val, desc, 1, end);
		      curr
			= read_and_display_attr_value (0, val, 0,
						       start, curr, end, 0, 0,
						       offset_size, version,
						       NULL, 0, section,
						       NULL, ' ', -1);
		      if (n != nargs - 1)
			printf (",");
		    }
		  printf ("\n");
		}
	      break;
	    }
	}

      printf ("\n");
    }

  return 1;
}