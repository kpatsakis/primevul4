display_debug_lines_decoded (struct dwarf_section *  section,
			     unsigned char *         start,
			     unsigned char *         data,
			     unsigned char *         end,
			     void *                  fileptr)
{
  static DWARF2_Internal_LineInfo saved_linfo;

  introduce (section, false);

  while (data < end)
    {
      /* This loop amounts to one iteration per compilation unit.  */
      DWARF2_Internal_LineInfo linfo;
      unsigned char *standard_opcodes;
      unsigned char *end_of_sequence;
      int i;
      File_Entry *file_table = NULL;
      unsigned int n_files = 0;
      unsigned char **directory_table = NULL;
      dwarf_vma n_directories = 0;

      if (startswith (section->name, ".debug_line.")
	  /* Note: the following does not apply to .debug_line.dwo sections.
	     These are full debug_line sections.  */
	  && strcmp (section->name, ".debug_line.dwo") != 0)
	{
	  /* See comment in display_debug_lines_raw().  */
	  end_of_sequence = end;
	  standard_opcodes = NULL;
	  linfo = saved_linfo;
	  /* PR 17531: file: 0522b371.  */
	  if (linfo.li_line_range == 0)
	    {
	      warn (_("Partial .debug_line. section encountered without a prior full .debug_line section\n"));
	      return 0;
	    }
	  reset_state_machine (linfo.li_default_is_stmt);
	}
      else
	{
	  unsigned char *hdrptr;

	  if ((hdrptr = read_debug_line_header (section, data, end, & linfo,
						& end_of_sequence)) == NULL)
	      return 0;

	  /* PR 17531: file: 0522b371.  */
	  if (linfo.li_line_range == 0)
	    {
	      warn (_("Line range of 0 is invalid, using 1 instead\n"));
	      linfo.li_line_range = 1;
	    }
	  reset_state_machine (linfo.li_default_is_stmt);

	  /* Save a pointer to the contents of the Opcodes table.  */
	  standard_opcodes = hdrptr;

	  /* Traverse the Directory table just to count entries.  */
	  data = standard_opcodes + linfo.li_opcode_base - 1;
	  /* PR 20440 */
	  if (data >= end)
	    {
	      warn (_("opcode base of %d extends beyond end of section\n"),
		    linfo.li_opcode_base);
	      return 0;
	    }

	  if (linfo.li_version >= 5)
	    {
	      unsigned char *format_start, format_count, *format;
	      dwarf_vma formati, entryi;

	      load_debug_section_with_follow (line_str, fileptr);

	      /* Skip directories format.  */
	      SAFE_BYTE_GET_AND_INC (format_count, data, 1, end);
	      if (do_checks && format_count > 1)
		warn (_("Unexpectedly large number of columns in the directory name table (%u)\n"),
		      format_count);
	      format_start = data;
	      for (formati = 0; formati < format_count; formati++)
		{
		  SKIP_ULEB (data, end);
		  SKIP_ULEB (data, end);
		}

	      READ_ULEB (n_directories, data, end);
	      if (data >= end)
		{
		  warn (_("Corrupt directories list\n"));
		  break;
		}

	      if (n_directories == 0)
		directory_table = NULL;
	      else
		directory_table = (unsigned char **)
		  xmalloc (n_directories * sizeof (unsigned char *));

	      for (entryi = 0; entryi < n_directories; entryi++)
		{
		  unsigned char **pathp = &directory_table[entryi];

		  format = format_start;
		  for (formati = 0; formati < format_count; formati++)
		    {
		      dwarf_vma content_type, form;
		      dwarf_vma uvalue;

		      READ_ULEB (content_type, format, end);
		      READ_ULEB (form, format, end);
		      if (data >= end)
			{
			  warn (_("Corrupt directories list\n"));
			  break;
			}
		      switch (content_type)
			{
			case DW_LNCT_path:
			  switch (form)
			    {
			    case DW_FORM_string:
			      *pathp = data;
			      break;
			    case DW_FORM_line_strp:
			      SAFE_BYTE_GET (uvalue, data, linfo.li_offset_size,
					     end);
			      /* Remove const by the cast.  */
			      *pathp = (unsigned char *)
				       fetch_indirect_line_string (uvalue);
			      break;
			    }
			  break;
			}
		      data = read_and_display_attr_value (0, form, 0, start,
							  data, end, 0, 0,
							  linfo.li_offset_size,
							  linfo.li_version,
							  NULL, 1, section,
							  NULL, '\t', -1);
		    }
		  if (data >= end)
		    {
		      warn (_("Corrupt directories list\n"));
		      break;
		    }
		}

	      /* Skip files format.  */
	      SAFE_BYTE_GET_AND_INC (format_count, data, 1, end);
	      if (do_checks && format_count > 5)
		warn (_("Unexpectedly large number of columns in the file name table (%u)\n"),
		      format_count);
	      format_start = data;
	      for (formati = 0; formati < format_count; formati++)
		{
		  SKIP_ULEB (data, end);
		  SKIP_ULEB (data, end);
		}

	      READ_ULEB (n_files, data, end);
	      if (data >= end && n_files > 0)
		{
		  warn (_("Corrupt file name list\n"));
		  break;
		}

	      if (n_files == 0)
		file_table = NULL;
	      else
		file_table = (File_Entry *) xcalloc (1, n_files
						     * sizeof (File_Entry));

	      for (entryi = 0; entryi < n_files; entryi++)
		{
		  File_Entry *file = &file_table[entryi];

		  format = format_start;
		  for (formati = 0; formati < format_count; formati++)
		    {
		      dwarf_vma content_type, form;
		      dwarf_vma uvalue;
		      unsigned char *tmp;

		      READ_ULEB (content_type, format, end);
		      READ_ULEB (form, format, end);
		      if (data >= end)
			{
			  warn (_("Corrupt file name list\n"));
			  break;
			}
		      switch (content_type)
			{
			case DW_LNCT_path:
			  switch (form)
			    {
			    case DW_FORM_string:
			      file->name = data;
			      break;
			    case DW_FORM_line_strp:
			      SAFE_BYTE_GET (uvalue, data, linfo.li_offset_size,
					     end);
			      /* Remove const by the cast.  */
			      file->name = (unsigned char *)
					   fetch_indirect_line_string (uvalue);
			      break;
			    }
			  break;
			case DW_LNCT_directory_index:
			  switch (form)
			    {
			    case DW_FORM_data1:
			      SAFE_BYTE_GET (file->directory_index, data, 1,
					     end);
			      break;
			    case DW_FORM_data2:
			      SAFE_BYTE_GET (file->directory_index, data, 2,
					     end);
			      break;
			    case DW_FORM_udata:
			      tmp = data;
			      READ_ULEB (file->directory_index, tmp, end);
			      break;
			    }
			  break;
			}
		      data = read_and_display_attr_value (0, form, 0, start,
							  data, end, 0, 0,
							  linfo.li_offset_size,
							  linfo.li_version,
							  NULL, 1, section,
							  NULL, '\t', -1);
		    }
		  if (data >= end)
		    {
		      warn (_("Corrupt file name list\n"));
		      break;
		    }
		}
	    }
	  else
	    {
	      if (*data != 0)
		{
		  unsigned char *ptr_directory_table = data;

		  while (data < end && *data != 0)
		    {
		      data += strnlen ((char *) data, end - data);
		      if (data < end)
			data++;
		      n_directories++;
		    }

		  /* PR 20440 */
		  if (data >= end)
		    {
		      warn (_("directory table ends unexpectedly\n"));
		      n_directories = 0;
		      break;
		    }

		  /* Go through the directory table again to save the directories.  */
		  directory_table = (unsigned char **)
		    xmalloc (n_directories * sizeof (unsigned char *));

		  i = 0;
		  while (*ptr_directory_table != 0)
		    {
		      directory_table[i] = ptr_directory_table;
		      ptr_directory_table
			+= strlen ((char *) ptr_directory_table) + 1;
		      i++;
		    }
		}
	      /* Skip the NUL at the end of the table.  */
	      data++;

	      /* Traverse the File Name table just to count the entries.  */
	      if (data < end && *data != 0)
		{
		  unsigned char *ptr_file_name_table = data;

		  while (data < end && *data != 0)
		    {
		      /* Skip Name, directory index, last modification
			 time and length of file.  */
		      data += strnlen ((char *) data, end - data);
		      if (data < end)
			data++;
		      SKIP_ULEB (data, end);
		      SKIP_ULEB (data, end);
		      SKIP_ULEB (data, end);
		      n_files++;
		    }

		  if (data >= end)
		    {
		      warn (_("file table ends unexpectedly\n"));
		      n_files = 0;
		      break;
		    }

		  /* Go through the file table again to save the strings.  */
		  file_table = (File_Entry *) xmalloc (n_files * sizeof (File_Entry));

		  i = 0;
		  while (*ptr_file_name_table != 0)
		    {
		      file_table[i].name = ptr_file_name_table;
		      ptr_file_name_table
			+= strlen ((char *) ptr_file_name_table) + 1;

		      /* We are not interested in directory, time or size.  */
		      READ_ULEB (file_table[i].directory_index,
				 ptr_file_name_table, end);
		      READ_ULEB (file_table[i].modification_date,
				 ptr_file_name_table, end);
		      READ_ULEB (file_table[i].length,
				 ptr_file_name_table, end);
		      i++;
		    }
		  i = 0;
		}

	      /* Skip the NUL at the end of the table.  */
	      data++;
	    }

	  /* Print the Compilation Unit's name and a header.  */
	  if (file_table == NULL)
	    printf (_("CU: No directory table\n"));
	  else if (directory_table == NULL)
	    printf (_("CU: %s:\n"), file_table[0].name);
	  else
	    {
	      unsigned int ix = file_table[0].directory_index;
	      const char *directory;

	      if (ix == 0)
		directory = ".";
	      /* PR 20439 */
	      else if (n_directories == 0)
		directory = _("<unknown>");
	      else if (ix > n_directories)
		{
		  warn (_("directory index %u > number of directories %s\n"),
			ix, dwarf_vmatoa ("u", n_directories));
		  directory = _("<corrupt>");
		}
	      else
		directory = (char *) directory_table[ix - 1];

	      if (do_wide)
		printf (_("CU: %s/%s:\n"), directory, file_table[0].name);
	      else
		printf ("%s:\n", file_table[0].name);
	    }

	  if (n_files > 0)
	    printf (_("File name                            Line number    Starting address    View    Stmt\n"));
	  else
	    printf (_("CU: Empty file name table\n"));
	  saved_linfo = linfo;
	}

      /* This loop iterates through the Dwarf Line Number Program.  */
      while (data < end_of_sequence)
	{
	  unsigned char op_code;
	  int xop;
	  int adv;
	  unsigned long int uladv;
	  int is_special_opcode = 0;

	  op_code = *data++;
	  xop = op_code;

	  if (op_code >= linfo.li_opcode_base)
	    {
	      op_code -= linfo.li_opcode_base;
	      uladv = (op_code / linfo.li_line_range);
	      if (linfo.li_max_ops_per_insn == 1)
		{
		  uladv *= linfo.li_min_insn_length;
		  state_machine_regs.address += uladv;
		  if (uladv)
		    state_machine_regs.view = 0;
		}
	      else
		{
		  unsigned addrdelta
		    = ((state_machine_regs.op_index + uladv)
		       / linfo.li_max_ops_per_insn)
		    * linfo.li_min_insn_length;
		  state_machine_regs.address
		    += addrdelta;
		  state_machine_regs.op_index
		    = (state_machine_regs.op_index + uladv)
		    % linfo.li_max_ops_per_insn;
		  if (addrdelta)
		    state_machine_regs.view = 0;
		}

	      adv = (op_code % linfo.li_line_range) + linfo.li_line_base;
	      state_machine_regs.line += adv;
	      is_special_opcode = 1;
	      /* Increment view after printing this row.  */
	    }
	  else
	    switch (op_code)
	      {
	      case DW_LNS_extended_op:
		{
		  unsigned int ext_op_code_len;
		  unsigned char ext_op_code;
		  unsigned char *op_code_end;
		  unsigned char *op_code_data = data;

		  READ_ULEB (ext_op_code_len, op_code_data, end_of_sequence);
		  op_code_end = op_code_data + ext_op_code_len;
		  if (ext_op_code_len == 0 || op_code_end > end_of_sequence)
		    {
		      warn (_("Badly formed extended line op encountered!\n"));
		      break;
		    }
		  ext_op_code = *op_code_data++;
		  xop = ext_op_code;
		  xop = -xop;

		  switch (ext_op_code)
		    {
		    case DW_LNE_end_sequence:
		      /* Reset stuff after printing this row.  */
		      break;
		    case DW_LNE_set_address:
		      SAFE_BYTE_GET_AND_INC (state_machine_regs.address,
					     op_code_data,
					     op_code_end - op_code_data,
					     op_code_end);
		      state_machine_regs.op_index = 0;
		      state_machine_regs.view = 0;
		      break;
		    case DW_LNE_define_file:
		      file_table = (File_Entry *) xrealloc
			(file_table, (n_files + 1) * sizeof (File_Entry));

		      ++state_machine_regs.last_file_entry;
		      /* Source file name.  */
		      file_table[n_files].name = op_code_data;
		      op_code_data += strlen ((char *) op_code_data) + 1;
		      /* Directory index.  */
		      READ_ULEB (file_table[n_files].directory_index,
				 op_code_data, op_code_end);
		      /* Last modification time.  */
		      READ_ULEB (file_table[n_files].modification_date,
				 op_code_data, op_code_end);
		      /* File length.  */
		      READ_ULEB (file_table[n_files].length,
				 op_code_data, op_code_end);
		      n_files++;
		      break;

		    case DW_LNE_set_discriminator:
		    case DW_LNE_HP_set_sequence:
		      /* Simply ignored.  */
		      break;

		    default:
		      printf (_("UNKNOWN (%u): length %ld\n"),
			      ext_op_code, (long int) (op_code_data - data));
		      break;
		    }
		  data = op_code_end;
		  break;
		}
	      case DW_LNS_copy:
		/* Increment view after printing this row.  */
		break;

	      case DW_LNS_advance_pc:
		READ_ULEB (uladv, data, end);
		if (linfo.li_max_ops_per_insn == 1)
		  {
		    uladv *= linfo.li_min_insn_length;
		    state_machine_regs.address += uladv;
		    if (uladv)
		      state_machine_regs.view = 0;
		  }
		else
		  {
		    unsigned addrdelta
		      = ((state_machine_regs.op_index + uladv)
			 / linfo.li_max_ops_per_insn)
		      * linfo.li_min_insn_length;
		    state_machine_regs.address
		      += addrdelta;
		    state_machine_regs.op_index
		      = (state_machine_regs.op_index + uladv)
		      % linfo.li_max_ops_per_insn;
		    if (addrdelta)
		      state_machine_regs.view = 0;
		  }
		break;

	      case DW_LNS_advance_line:
		READ_SLEB (adv, data, end);
		state_machine_regs.line += adv;
		break;

	      case DW_LNS_set_file:
		READ_ULEB (uladv, data, end);
		state_machine_regs.file = uladv;

		{
		  unsigned file = state_machine_regs.file;
		  unsigned dir;

		  if (linfo.li_version < 5)
		    --file;
		  if (file_table == NULL || n_files == 0)
		    printf (_("\n [Use file table entry %d]\n"), file);
		  /* PR 20439 */
		  else if (file >= n_files)
		    {
		      warn (_("file index %u > number of files %u\n"), file, n_files);
		      printf (_("\n <over large file table index %u>"), file);
		    }
		  else if ((dir = file_table[file].directory_index) == 0)
		    /* If directory index is 0, that means current directory.  */
		    printf ("\n./%s:[++]\n", file_table[file].name);
		  else if (directory_table == NULL || n_directories == 0)
		    printf (_("\n [Use file %s in directory table entry %d]\n"),
			    file_table[file].name, dir);
		  /* PR 20439 */
		  else if (dir > n_directories)
		    {
		      warn (_("directory index %u > number of directories %s\n"),
			    dir, dwarf_vmatoa ("u", n_directories));
		      printf (_("\n <over large directory table entry %u>\n"), dir);
		    }
		  else
		    printf ("\n%s/%s:\n",
			    /* The directory index starts counting at 1.  */
			    directory_table[dir - 1], file_table[file].name);
		}
		break;

	      case DW_LNS_set_column:
		READ_ULEB (uladv, data, end);
		state_machine_regs.column = uladv;
		break;

	      case DW_LNS_negate_stmt:
		adv = state_machine_regs.is_stmt;
		adv = ! adv;
		state_machine_regs.is_stmt = adv;
		break;

	      case DW_LNS_set_basic_block:
		state_machine_regs.basic_block = 1;
		break;

	      case DW_LNS_const_add_pc:
		uladv = ((255 - linfo.li_opcode_base) / linfo.li_line_range);
		if (linfo.li_max_ops_per_insn == 1)
		  {
		    uladv *= linfo.li_min_insn_length;
		    state_machine_regs.address += uladv;
		    if (uladv)
		      state_machine_regs.view = 0;
		  }
		else
		  {
		    unsigned addrdelta
		      = ((state_machine_regs.op_index + uladv)
			 / linfo.li_max_ops_per_insn)
		      * linfo.li_min_insn_length;
		    state_machine_regs.address
		      += addrdelta;
		    state_machine_regs.op_index
		      = (state_machine_regs.op_index + uladv)
		      % linfo.li_max_ops_per_insn;
		    if (addrdelta)
		      state_machine_regs.view = 0;
		  }
		break;

	      case DW_LNS_fixed_advance_pc:
		SAFE_BYTE_GET_AND_INC (uladv, data, 2, end);
		state_machine_regs.address += uladv;
		state_machine_regs.op_index = 0;
		/* Do NOT reset view.  */
		break;

	      case DW_LNS_set_prologue_end:
		break;

	      case DW_LNS_set_epilogue_begin:
		break;

	      case DW_LNS_set_isa:
		READ_ULEB (uladv, data, end);
		printf (_("  Set ISA to %lu\n"), uladv);
		break;

	      default:
		printf (_("  Unknown opcode %d with operands: "), op_code);

		if (standard_opcodes != NULL)
		  for (i = standard_opcodes[op_code - 1]; i > 0 ; --i)
		    {
		      dwarf_vma val;

		      READ_ULEB (val, data, end);
		      printf ("0x%s%s", dwarf_vmatoa ("x", val),
			      i == 1 ? "" : ", ");
		    }
		putchar ('\n');
		break;
	      }

	  /* Only Special opcodes, DW_LNS_copy and DW_LNE_end_sequence adds a row
	     to the DWARF address/line matrix.  */
	  if ((is_special_opcode) || (xop == -DW_LNE_end_sequence)
	      || (xop == DW_LNS_copy))
	    {
	      const unsigned int MAX_FILENAME_LENGTH = 35;
	      char *fileName;
	      char *newFileName = NULL;
	      size_t fileNameLength;

	      if (file_table)
		{
		  unsigned indx = state_machine_regs.file;

		  if (linfo.li_version < 5)
		    --indx;
		  /* PR 20439  */
		  if (indx >= n_files)
		    {
		      warn (_("corrupt file index %u encountered\n"), indx);
		      fileName = _("<corrupt>");
		    }
		  else
		    fileName = (char *) file_table[indx].name;
		}
	      else
		fileName = _("<unknown>");

	      fileNameLength = strlen (fileName);
	      newFileName = fileName;
	      if (fileNameLength > MAX_FILENAME_LENGTH && !do_wide)
		{
		  newFileName = (char *) xmalloc (MAX_FILENAME_LENGTH + 1);
		  /* Truncate file name */
		  memcpy (newFileName,
			  fileName + fileNameLength - MAX_FILENAME_LENGTH,
			  MAX_FILENAME_LENGTH);
		  newFileName[MAX_FILENAME_LENGTH] = 0;
		}

	      /* A row with end_seq set to true has a meaningful address, but
		 the other information in the same row is not significant.
		 In such a row, print line as "-", and don't print
		 view/is_stmt.  */
	      if (!do_wide || fileNameLength <= MAX_FILENAME_LENGTH)
		{
		  if (linfo.li_max_ops_per_insn == 1)
		    {
		      if (xop == -DW_LNE_end_sequence)
			printf ("%-35s  %11s  %#18" DWARF_VMA_FMT "x",
				newFileName, "-",
				state_machine_regs.address);
		      else
			printf ("%-35s  %11d  %#18" DWARF_VMA_FMT "x",
				newFileName, state_machine_regs.line,
				state_machine_regs.address);
		    }
		  else
		    {
		      if (xop == -DW_LNE_end_sequence)
			printf ("%-35s  %11s  %#18" DWARF_VMA_FMT "x[%d]",
				newFileName, "-",
				state_machine_regs.address,
				state_machine_regs.op_index);
		      else
			printf ("%-35s  %11d  %#18" DWARF_VMA_FMT "x[%d]",
				newFileName, state_machine_regs.line,
				state_machine_regs.address,
				state_machine_regs.op_index);
		    }
		}
	      else
		{
		  if (linfo.li_max_ops_per_insn == 1)
		    {
		      if (xop == -DW_LNE_end_sequence)
			printf ("%s  %11s  %#18" DWARF_VMA_FMT "x",
				newFileName, "-",
				state_machine_regs.address);
		      else
			printf ("%s  %11d  %#18" DWARF_VMA_FMT "x",
				newFileName, state_machine_regs.line,
				state_machine_regs.address);
		    }			
		  else
		    {
		      if (xop == -DW_LNE_end_sequence)
			printf ("%s  %11s  %#18" DWARF_VMA_FMT "x[%d]",
				newFileName, "-",
				state_machine_regs.address,
				state_machine_regs.op_index);
		      else
			printf ("%s  %11d  %#18" DWARF_VMA_FMT "x[%d]",
				newFileName, state_machine_regs.line,
				state_machine_regs.address,
				state_machine_regs.op_index);
		    }
		}

	      if (xop != -DW_LNE_end_sequence)
		{
		  if (state_machine_regs.view)
		    printf ("  %6u", state_machine_regs.view);
		  else
		    printf ("        ");

		  if (state_machine_regs.is_stmt)
		    printf ("       x");
		}

	      putchar ('\n');
	      state_machine_regs.view++;

	      if (xop == -DW_LNE_end_sequence)
		{
		  reset_state_machine (linfo.li_default_is_stmt);
		  putchar ('\n');
		}

	      if (newFileName != fileName)
		free (newFileName);
	    }
	}

      if (file_table)
	{
	  free (file_table);
	  file_table = NULL;
	  n_files = 0;
	}

      if (directory_table)
	{
	  free (directory_table);
	  directory_table = NULL;
	  n_directories = 0;
	}

      putchar ('\n');
    }

  return 1;
}