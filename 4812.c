process_extended_line_op (unsigned char * data,
			  int is_stmt,
			  unsigned char * end)
{
  unsigned char op_code;
  size_t len, header_len;
  unsigned char *name;
  unsigned char *orig_data = data;
  dwarf_vma adr, val;

  READ_ULEB (len, data, end);
  header_len = data - orig_data;

  if (len == 0 || data >= end || len > (size_t) (end - data))
    {
      warn (_("Badly formed extended line op encountered!\n"));
      return header_len;
    }

  op_code = *data++;

  printf (_("  Extended opcode %d: "), op_code);

  switch (op_code)
    {
    case DW_LNE_end_sequence:
      printf (_("End of Sequence\n\n"));
      reset_state_machine (is_stmt);
      break;

    case DW_LNE_set_address:
      /* PR 17512: file: 002-100480-0.004.  */
      if (len - 1 > 8)
	{
	  warn (_("Length (%lu) of DW_LNE_set_address op is too long\n"),
		(unsigned long) len - 1);
	  adr = 0;
	}
      else
	SAFE_BYTE_GET (adr, data, len - 1, end);
      printf (_("set Address to 0x%s\n"), dwarf_vmatoa ("x", adr));
      state_machine_regs.address = adr;
      state_machine_regs.view = 0;
      state_machine_regs.op_index = 0;
      break;

    case DW_LNE_define_file:
      printf (_("define new File Table entry\n"));
      printf (_("  Entry\tDir\tTime\tSize\tName\n"));
      printf ("   %d\t", ++state_machine_regs.last_file_entry);

      {
	size_t l;

	name = data;
	l = strnlen ((char *) data, end - data);
	data += l;
	if (data < end)
	  data++;
	READ_ULEB (val, data, end);
	printf ("%s\t", dwarf_vmatoa ("u", val));
	READ_ULEB (val, data, end);
	printf ("%s\t", dwarf_vmatoa ("u", val));
	READ_ULEB (val, data, end);
	printf ("%s\t", dwarf_vmatoa ("u", val));
	printf ("%.*s\n\n", (int) l, name);
      }

      if (((size_t) (data - orig_data) != len + header_len) || data >= end)
	warn (_("DW_LNE_define_file: Bad opcode length\n"));
      break;

    case DW_LNE_set_discriminator:
      READ_ULEB (val, data, end);
      printf (_("set Discriminator to %s\n"), dwarf_vmatoa ("u", val));
      break;

    /* HP extensions.  */
    case DW_LNE_HP_negate_is_UV_update:
      printf ("DW_LNE_HP_negate_is_UV_update\n");
      break;
    case DW_LNE_HP_push_context:
      printf ("DW_LNE_HP_push_context\n");
      break;
    case DW_LNE_HP_pop_context:
      printf ("DW_LNE_HP_pop_context\n");
      break;
    case DW_LNE_HP_set_file_line_column:
      printf ("DW_LNE_HP_set_file_line_column\n");
      break;
    case DW_LNE_HP_set_routine_name:
      printf ("DW_LNE_HP_set_routine_name\n");
      break;
    case DW_LNE_HP_set_sequence:
      printf ("DW_LNE_HP_set_sequence\n");
      break;
    case DW_LNE_HP_negate_post_semantics:
      printf ("DW_LNE_HP_negate_post_semantics\n");
      break;
    case DW_LNE_HP_negate_function_exit:
      printf ("DW_LNE_HP_negate_function_exit\n");
      break;
    case DW_LNE_HP_negate_front_end_logical:
      printf ("DW_LNE_HP_negate_front_end_logical\n");
      break;
    case DW_LNE_HP_define_proc:
      printf ("DW_LNE_HP_define_proc\n");
      break;
    case DW_LNE_HP_source_file_correlation:
      {
	unsigned char *edata = data + len - 1;

	printf ("DW_LNE_HP_source_file_correlation\n");

	while (data < edata)
	  {
	    unsigned int opc;

	    READ_ULEB (opc, data, edata);

	    switch (opc)
	      {
	      case DW_LNE_HP_SFC_formfeed:
		printf ("    DW_LNE_HP_SFC_formfeed\n");
		break;
	      case DW_LNE_HP_SFC_set_listing_line:
		READ_ULEB (val, data, edata);
		printf ("    DW_LNE_HP_SFC_set_listing_line (%s)\n",
			dwarf_vmatoa ("u", val));
		break;
	      case DW_LNE_HP_SFC_associate:
		printf ("    DW_LNE_HP_SFC_associate ");
		READ_ULEB (val, data, edata);
		printf ("(%s", dwarf_vmatoa ("u", val));
		READ_ULEB (val, data, edata);
		printf (",%s", dwarf_vmatoa ("u", val));
		READ_ULEB (val, data, edata);
		printf (",%s)\n", dwarf_vmatoa ("u", val));
		break;
	      default:
		printf (_("    UNKNOWN DW_LNE_HP_SFC opcode (%u)\n"), opc);
		data = edata;
		break;
	      }
	  }
      }
      break;

    default:
      {
	unsigned int rlen = len - 1;

	if (op_code >= DW_LNE_lo_user
	    /* The test against DW_LNW_hi_user is redundant due to
	       the limited range of the unsigned char data type used
	       for op_code.  */
	    /*&& op_code <= DW_LNE_hi_user*/)
	  printf (_("user defined: "));
	else
	  printf (_("UNKNOWN: "));
	printf (_("length %d ["), rlen);
	for (; rlen; rlen--)
	  printf (" %02x", *data++);
	printf ("]\n");
      }
      break;
    }

  return len + header_len;
}