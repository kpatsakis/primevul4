display_debug_lines_raw (struct dwarf_section *  section,
			 unsigned char *         data,
			 unsigned char *         end,
			 void *                  file)
{
  unsigned char *start = section->start;
  int verbose_view = 0;

  introduce (section, true);

  while (data < end)
    {
      static DWARF2_Internal_LineInfo saved_linfo;
      DWARF2_Internal_LineInfo linfo;
      unsigned char *standard_opcodes;
      unsigned char *end_of_sequence;
      int i;

      if (startswith (section->name, ".debug_line.")
	  /* Note: the following does not apply to .debug_line.dwo sections.
	     These are full debug_line sections.  */
	  && strcmp (section->name, ".debug_line.dwo") != 0)
	{
	  /* Sections named .debug_line.<foo> are fragments of a .debug_line
	     section containing just the Line Number Statements.  They are
	     created by the assembler and intended to be used alongside gcc's
	     -ffunction-sections command line option.  When the linker's
	     garbage collection decides to discard a .text.<foo> section it
	     can then also discard the line number information in .debug_line.<foo>.

	     Since the section is a fragment it does not have the details
	     needed to fill out a LineInfo structure, so instead we use the
	     details from the last full debug_line section that we processed.  */
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
	  unsigned char * hdrptr;

	  if ((hdrptr = read_debug_line_header (section, data, end, & linfo,
						& end_of_sequence)) == NULL)
	    return 0;

	  printf (_("  Offset:                      0x%lx\n"), (long)(data - start));
	  printf (_("  Length:                      %ld\n"), (long) linfo.li_length);
	  printf (_("  DWARF Version:               %d\n"), linfo.li_version);
	  if (linfo.li_version >= 5)
	    {
	      printf (_("  Address size (bytes):        %d\n"), linfo.li_address_size);
	      printf (_("  Segment selector (bytes):    %d\n"), linfo.li_segment_size);
	    }
	  printf (_("  Prologue Length:             %d\n"), (int) linfo.li_prologue_length);
	  printf (_("  Minimum Instruction Length:  %d\n"), linfo.li_min_insn_length);
	  if (linfo.li_version >= 4)
	    printf (_("  Maximum Ops per Instruction: %d\n"), linfo.li_max_ops_per_insn);
	  printf (_("  Initial value of 'is_stmt':  %d\n"), linfo.li_default_is_stmt);
	  printf (_("  Line Base:                   %d\n"), linfo.li_line_base);
	  printf (_("  Line Range:                  %d\n"), linfo.li_line_range);
	  printf (_("  Opcode Base:                 %d\n"), linfo.li_opcode_base);

	  /* PR 17512: file: 1665-6428-0.004.  */
	  if (linfo.li_line_range == 0)
	    {
	      warn (_("Line range of 0 is invalid, using 1 instead\n"));
	      linfo.li_line_range = 1;
	    }

	  reset_state_machine (linfo.li_default_is_stmt);

	  /* Display the contents of the Opcodes table.  */
	  standard_opcodes = hdrptr;

	  /* PR 17512: file: 002-417945-0.004.  */
	  if (standard_opcodes + linfo.li_opcode_base >= end)
	    {
	      warn (_("Line Base extends beyond end of section\n"));
	      return 0;
	    }

	  printf (_("\n Opcodes:\n"));

	  for (i = 1; i < linfo.li_opcode_base; i++)
	    printf (ngettext ("  Opcode %d has %d arg\n",
			      "  Opcode %d has %d args\n",
			      standard_opcodes[i - 1]),
		    i, standard_opcodes[i - 1]);

	  /* Display the contents of the Directory table.  */
	  data = standard_opcodes + linfo.li_opcode_base - 1;

	  if (linfo.li_version >= 5)
	    {
	      load_debug_section_with_follow (line_str, file);

	      data = display_formatted_table (data, start, end, &linfo, section,
					      true);
	      data = display_formatted_table (data, start, end, &linfo, section,
					      false);
	    }
	  else
	    {
	      if (*data == 0)
		printf (_("\n The Directory Table is empty.\n"));
	      else
		{
		  unsigned int last_dir_entry = 0;

		  printf (_("\n The Directory Table (offset 0x%lx):\n"),
			  (long)(data - start));

		  while (data < end && *data != 0)
		    {
		      printf ("  %d\t%.*s\n", ++last_dir_entry, (int) (end - data), data);

		      data += strnlen ((char *) data, end - data);
		      if (data < end)
			data++;
		    }

		  /* PR 17512: file: 002-132094-0.004.  */
		  if (data >= end - 1)
		    break;
		}

	      /* Skip the NUL at the end of the table.  */
	      if (data < end)
		data++;

	      /* Display the contents of the File Name table.  */
	      if (data >= end || *data == 0)
		printf (_("\n The File Name Table is empty.\n"));
	      else
		{
		  printf (_("\n The File Name Table (offset 0x%lx):\n"),
			  (long)(data - start));
		  printf (_("  Entry\tDir\tTime\tSize\tName\n"));

		  while (data < end && *data != 0)
		    {
		      unsigned char *name;
		      dwarf_vma val;

		      printf ("  %d\t", ++state_machine_regs.last_file_entry);
		      name = data;
		      data += strnlen ((char *) data, end - data);
		      if (data < end)
			data++;

		      READ_ULEB (val, data, end);
		      printf ("%s\t", dwarf_vmatoa ("u", val));
		      READ_ULEB (val, data, end);
		      printf ("%s\t", dwarf_vmatoa ("u", val));
		      READ_ULEB (val, data, end);
		      printf ("%s\t", dwarf_vmatoa ("u", val));
		      printf ("%.*s\n", (int)(end - name), name);

		      if (data >= end)
			{
			  warn (_("Corrupt file name table entry\n"));
			  break;
			}
		    }
		}

	      /* Skip the NUL at the end of the table.  */
	      if (data < end)
		data++;
	    }

	  putchar ('\n');
	  saved_linfo = linfo;
	}

      /* Now display the statements.  */
      if (data >= end_of_sequence)
	printf (_(" No Line Number Statements.\n"));
      else
	{
	  printf (_(" Line Number Statements:\n"));

	  while (data < end_of_sequence)
	    {
	      unsigned char op_code;
	      dwarf_signed_vma adv;
	      dwarf_vma uladv;

	      printf ("  [0x%08lx]", (long)(data - start));

	      op_code = *data++;

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
		      printf (_("  Special opcode %d: "
				"advance Address by %s to 0x%s%s"),
			      op_code, dwarf_vmatoa ("u", uladv),
			      dwarf_vmatoa ("x", state_machine_regs.address),
			      verbose_view && uladv
			      ? _(" (reset view)") : "");
		    }
		  else
		    {
		      unsigned addrdelta
			= ((state_machine_regs.op_index + uladv)
			    / linfo.li_max_ops_per_insn)
			* linfo.li_min_insn_length;

		      state_machine_regs.address += addrdelta;
		      state_machine_regs.op_index
			= (state_machine_regs.op_index + uladv)
			% linfo.li_max_ops_per_insn;
		      if (addrdelta)
			state_machine_regs.view = 0;
		      printf (_("  Special opcode %d: "
				"advance Address by %s to 0x%s[%d]%s"),
			      op_code, dwarf_vmatoa ("u", uladv),
			      dwarf_vmatoa ("x", state_machine_regs.address),
			      state_machine_regs.op_index,
			      verbose_view && addrdelta
			      ? _(" (reset view)") : "");
		    }
		  adv = (op_code % linfo.li_line_range) + linfo.li_line_base;
		  state_machine_regs.line += adv;
		  printf (_(" and Line by %s to %d"),
			  dwarf_vmatoa ("d", adv), state_machine_regs.line);
		  if (verbose_view || state_machine_regs.view)
		    printf (_(" (view %u)\n"), state_machine_regs.view);
		  else
		    putchar ('\n');
		  state_machine_regs.view++;
		}
	      else
		switch (op_code)
		  {
		  case DW_LNS_extended_op:
		    data += process_extended_line_op (data,
						      linfo.li_default_is_stmt,
						      end);
		    break;

		  case DW_LNS_copy:
		    printf (_("  Copy"));
		    if (verbose_view || state_machine_regs.view)
		      printf (_(" (view %u)\n"), state_machine_regs.view);
		    else
		      putchar ('\n');
		    state_machine_regs.view++;
		    break;

		  case DW_LNS_advance_pc:
		    READ_ULEB (uladv, data, end);
		    if (linfo.li_max_ops_per_insn == 1)
		      {
			uladv *= linfo.li_min_insn_length;
			state_machine_regs.address += uladv;
			if (uladv)
			  state_machine_regs.view = 0;
			printf (_("  Advance PC by %s to 0x%s%s\n"),
				dwarf_vmatoa ("u", uladv),
				dwarf_vmatoa ("x", state_machine_regs.address),
				verbose_view && uladv
				? _(" (reset view)") : "");
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
			printf (_("  Advance PC by %s to 0x%s[%d]%s\n"),
				dwarf_vmatoa ("u", uladv),
				dwarf_vmatoa ("x", state_machine_regs.address),
				state_machine_regs.op_index,
				verbose_view && addrdelta
				? _(" (reset view)") : "");
		      }
		    break;

		  case DW_LNS_advance_line:
		    READ_SLEB (adv, data, end);
		    state_machine_regs.line += adv;
		    printf (_("  Advance Line by %s to %d\n"),
			    dwarf_vmatoa ("d", adv),
			    state_machine_regs.line);
		    break;

		  case DW_LNS_set_file:
		    READ_ULEB (uladv, data, end);
		    printf (_("  Set File Name to entry %s in the File Name Table\n"),
			    dwarf_vmatoa ("u", uladv));
		    state_machine_regs.file = uladv;
		    break;

		  case DW_LNS_set_column:
		    READ_ULEB (uladv, data, end);
		    printf (_("  Set column to %s\n"),
			    dwarf_vmatoa ("u", uladv));
		    state_machine_regs.column = uladv;
		    break;

		  case DW_LNS_negate_stmt:
		    adv = state_machine_regs.is_stmt;
		    adv = ! adv;
		    printf (_("  Set is_stmt to %s\n"), dwarf_vmatoa ("d", adv));
		    state_machine_regs.is_stmt = adv;
		    break;

		  case DW_LNS_set_basic_block:
		    printf (_("  Set basic block\n"));
		    state_machine_regs.basic_block = 1;
		    break;

		  case DW_LNS_const_add_pc:
		    uladv = ((255 - linfo.li_opcode_base) / linfo.li_line_range);
		    if (linfo.li_max_ops_per_insn)
		      {
			uladv *= linfo.li_min_insn_length;
			state_machine_regs.address += uladv;
			if (uladv)
			  state_machine_regs.view = 0;
			printf (_("  Advance PC by constant %s to 0x%s%s\n"),
				dwarf_vmatoa ("u", uladv),
				dwarf_vmatoa ("x", state_machine_regs.address),
				verbose_view && uladv
				? _(" (reset view)") : "");
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
			printf (_("  Advance PC by constant %s to 0x%s[%d]%s\n"),
				dwarf_vmatoa ("u", uladv),
				dwarf_vmatoa ("x", state_machine_regs.address),
				state_machine_regs.op_index,
				verbose_view && addrdelta
				? _(" (reset view)") : "");
		      }
		    break;

		  case DW_LNS_fixed_advance_pc:
		    SAFE_BYTE_GET_AND_INC (uladv, data, 2, end);
		    state_machine_regs.address += uladv;
		    state_machine_regs.op_index = 0;
		    printf (_("  Advance PC by fixed size amount %s to 0x%s\n"),
			    dwarf_vmatoa ("u", uladv),
			    dwarf_vmatoa ("x", state_machine_regs.address));
		    /* Do NOT reset view.  */
		    break;

		  case DW_LNS_set_prologue_end:
		    printf (_("  Set prologue_end to true\n"));
		    break;

		  case DW_LNS_set_epilogue_begin:
		    printf (_("  Set epilogue_begin to true\n"));
		    break;

		  case DW_LNS_set_isa:
		    READ_ULEB (uladv, data, end);
		    printf (_("  Set ISA to %s\n"), dwarf_vmatoa ("u", uladv));
		    break;

		  default:
		    printf (_("  Unknown opcode %d with operands: "), op_code);

		    if (standard_opcodes != NULL)
		      for (i = standard_opcodes[op_code - 1]; i > 0 ; --i)
			{
			  READ_ULEB (uladv, data, end);
			  printf ("0x%s%s", dwarf_vmatoa ("x", uladv),
				  i == 1 ? "" : ", ");
			}
		    putchar ('\n');
		    break;
		  }
	    }
	  putchar ('\n');
	}
    }

  return 1;
}