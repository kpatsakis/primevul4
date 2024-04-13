decode_location_expression (unsigned char * data,
			    unsigned int pointer_size,
			    unsigned int offset_size,
			    int dwarf_version,
			    dwarf_vma length,
			    dwarf_vma cu_offset,
			    struct dwarf_section * section)
{
  unsigned op;
  dwarf_vma uvalue;
  dwarf_signed_vma svalue;
  unsigned char *end = data + length;
  int need_frame_base = 0;

  while (data < end)
    {
      op = *data++;

      switch (op)
	{
	case DW_OP_addr:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
	  printf ("DW_OP_addr: %s", dwarf_vmatoa ("x", uvalue));
	  break;
	case DW_OP_deref:
	  printf ("DW_OP_deref");
	  break;
	case DW_OP_const1u:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  printf ("DW_OP_const1u: %lu", (unsigned long) uvalue);
	  break;
	case DW_OP_const1s:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 1, end);
	  printf ("DW_OP_const1s: %ld", (long) svalue);
	  break;
	case DW_OP_const2u:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 2, end);
	  printf ("DW_OP_const2u: %lu", (unsigned long) uvalue);
	  break;
	case DW_OP_const2s:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 2, end);
	  printf ("DW_OP_const2s: %ld", (long) svalue);
	  break;
	case DW_OP_const4u:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
	  printf ("DW_OP_const4u: %lu", (unsigned long) uvalue);
	  break;
	case DW_OP_const4s:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 4, end);
	  printf ("DW_OP_const4s: %ld", (long) svalue);
	  break;
	case DW_OP_const8u:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
	  printf ("DW_OP_const8u: %lu ", (unsigned long) uvalue);
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
	  printf ("%lu", (unsigned long) uvalue);
	  break;
	case DW_OP_const8s:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 4, end);
	  printf ("DW_OP_const8s: %ld ", (long) svalue);
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 4, end);
	  printf ("%ld", (long) svalue);
	  break;
	case DW_OP_constu:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_constu: %s", dwarf_vmatoa ("u", uvalue));
	  break;
	case DW_OP_consts:
	  READ_SLEB (svalue, data, end);
	  printf ("DW_OP_consts: %s", dwarf_vmatoa ("d", svalue));
	  break;
	case DW_OP_dup:
	  printf ("DW_OP_dup");
	  break;
	case DW_OP_drop:
	  printf ("DW_OP_drop");
	  break;
	case DW_OP_over:
	  printf ("DW_OP_over");
	  break;
	case DW_OP_pick:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  printf ("DW_OP_pick: %ld", (unsigned long) uvalue);
	  break;
	case DW_OP_swap:
	  printf ("DW_OP_swap");
	  break;
	case DW_OP_rot:
	  printf ("DW_OP_rot");
	  break;
	case DW_OP_xderef:
	  printf ("DW_OP_xderef");
	  break;
	case DW_OP_abs:
	  printf ("DW_OP_abs");
	  break;
	case DW_OP_and:
	  printf ("DW_OP_and");
	  break;
	case DW_OP_div:
	  printf ("DW_OP_div");
	  break;
	case DW_OP_minus:
	  printf ("DW_OP_minus");
	  break;
	case DW_OP_mod:
	  printf ("DW_OP_mod");
	  break;
	case DW_OP_mul:
	  printf ("DW_OP_mul");
	  break;
	case DW_OP_neg:
	  printf ("DW_OP_neg");
	  break;
	case DW_OP_not:
	  printf ("DW_OP_not");
	  break;
	case DW_OP_or:
	  printf ("DW_OP_or");
	  break;
	case DW_OP_plus:
	  printf ("DW_OP_plus");
	  break;
	case DW_OP_plus_uconst:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_plus_uconst: %s", dwarf_vmatoa ("u", uvalue));
	  break;
	case DW_OP_shl:
	  printf ("DW_OP_shl");
	  break;
	case DW_OP_shr:
	  printf ("DW_OP_shr");
	  break;
	case DW_OP_shra:
	  printf ("DW_OP_shra");
	  break;
	case DW_OP_xor:
	  printf ("DW_OP_xor");
	  break;
	case DW_OP_bra:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 2, end);
	  printf ("DW_OP_bra: %ld", (long) svalue);
	  break;
	case DW_OP_eq:
	  printf ("DW_OP_eq");
	  break;
	case DW_OP_ge:
	  printf ("DW_OP_ge");
	  break;
	case DW_OP_gt:
	  printf ("DW_OP_gt");
	  break;
	case DW_OP_le:
	  printf ("DW_OP_le");
	  break;
	case DW_OP_lt:
	  printf ("DW_OP_lt");
	  break;
	case DW_OP_ne:
	  printf ("DW_OP_ne");
	  break;
	case DW_OP_skip:
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 2, end);
	  printf ("DW_OP_skip: %ld", (long) svalue);
	  break;

	case DW_OP_lit0:
	case DW_OP_lit1:
	case DW_OP_lit2:
	case DW_OP_lit3:
	case DW_OP_lit4:
	case DW_OP_lit5:
	case DW_OP_lit6:
	case DW_OP_lit7:
	case DW_OP_lit8:
	case DW_OP_lit9:
	case DW_OP_lit10:
	case DW_OP_lit11:
	case DW_OP_lit12:
	case DW_OP_lit13:
	case DW_OP_lit14:
	case DW_OP_lit15:
	case DW_OP_lit16:
	case DW_OP_lit17:
	case DW_OP_lit18:
	case DW_OP_lit19:
	case DW_OP_lit20:
	case DW_OP_lit21:
	case DW_OP_lit22:
	case DW_OP_lit23:
	case DW_OP_lit24:
	case DW_OP_lit25:
	case DW_OP_lit26:
	case DW_OP_lit27:
	case DW_OP_lit28:
	case DW_OP_lit29:
	case DW_OP_lit30:
	case DW_OP_lit31:
	  printf ("DW_OP_lit%d", op - DW_OP_lit0);
	  break;

	case DW_OP_reg0:
	case DW_OP_reg1:
	case DW_OP_reg2:
	case DW_OP_reg3:
	case DW_OP_reg4:
	case DW_OP_reg5:
	case DW_OP_reg6:
	case DW_OP_reg7:
	case DW_OP_reg8:
	case DW_OP_reg9:
	case DW_OP_reg10:
	case DW_OP_reg11:
	case DW_OP_reg12:
	case DW_OP_reg13:
	case DW_OP_reg14:
	case DW_OP_reg15:
	case DW_OP_reg16:
	case DW_OP_reg17:
	case DW_OP_reg18:
	case DW_OP_reg19:
	case DW_OP_reg20:
	case DW_OP_reg21:
	case DW_OP_reg22:
	case DW_OP_reg23:
	case DW_OP_reg24:
	case DW_OP_reg25:
	case DW_OP_reg26:
	case DW_OP_reg27:
	case DW_OP_reg28:
	case DW_OP_reg29:
	case DW_OP_reg30:
	case DW_OP_reg31:
	  printf ("DW_OP_reg%d (%s)", op - DW_OP_reg0,
		  regname (op - DW_OP_reg0, 1));
	  break;

	case DW_OP_breg0:
	case DW_OP_breg1:
	case DW_OP_breg2:
	case DW_OP_breg3:
	case DW_OP_breg4:
	case DW_OP_breg5:
	case DW_OP_breg6:
	case DW_OP_breg7:
	case DW_OP_breg8:
	case DW_OP_breg9:
	case DW_OP_breg10:
	case DW_OP_breg11:
	case DW_OP_breg12:
	case DW_OP_breg13:
	case DW_OP_breg14:
	case DW_OP_breg15:
	case DW_OP_breg16:
	case DW_OP_breg17:
	case DW_OP_breg18:
	case DW_OP_breg19:
	case DW_OP_breg20:
	case DW_OP_breg21:
	case DW_OP_breg22:
	case DW_OP_breg23:
	case DW_OP_breg24:
	case DW_OP_breg25:
	case DW_OP_breg26:
	case DW_OP_breg27:
	case DW_OP_breg28:
	case DW_OP_breg29:
	case DW_OP_breg30:
	case DW_OP_breg31:
	  READ_SLEB (svalue, data, end);
	  printf ("DW_OP_breg%d (%s): %s", op - DW_OP_breg0,
		  regname (op - DW_OP_breg0, 1), dwarf_vmatoa ("d", svalue));
	  break;

	case DW_OP_regx:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_regx: %s (%s)",
		  dwarf_vmatoa ("u", uvalue), regname (uvalue, 1));
	  break;
	case DW_OP_fbreg:
	  need_frame_base = 1;
	  READ_SLEB (svalue, data, end);
	  printf ("DW_OP_fbreg: %s", dwarf_vmatoa ("d", svalue));
	  break;
	case DW_OP_bregx:
	  READ_ULEB (uvalue, data, end);
	  READ_SLEB (svalue, data, end);
	  printf ("DW_OP_bregx: %s (%s) %s",
		  dwarf_vmatoa ("u", uvalue), regname (uvalue, 1),
		  dwarf_vmatoa ("d", svalue));
	  break;
	case DW_OP_piece:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_piece: %s", dwarf_vmatoa ("u", uvalue));
	  break;
	case DW_OP_deref_size:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  printf ("DW_OP_deref_size: %ld", (long) uvalue);
	  break;
	case DW_OP_xderef_size:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  printf ("DW_OP_xderef_size: %ld", (long) uvalue);
	  break;
	case DW_OP_nop:
	  printf ("DW_OP_nop");
	  break;

	  /* DWARF 3 extensions.  */
	case DW_OP_push_object_address:
	  printf ("DW_OP_push_object_address");
	  break;
	case DW_OP_call2:
	  /* FIXME: Strictly speaking for 64-bit DWARF3 files
	     this ought to be an 8-byte wide computation.  */
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 2, end);
	  printf ("DW_OP_call2: <0x%s>",
		  dwarf_vmatoa ("x", svalue + cu_offset));
	  break;
	case DW_OP_call4:
	  /* FIXME: Strictly speaking for 64-bit DWARF3 files
	     this ought to be an 8-byte wide computation.  */
	  SAFE_SIGNED_BYTE_GET_AND_INC (svalue, data, 4, end);
	  printf ("DW_OP_call4: <0x%s>",
		  dwarf_vmatoa ("x", svalue + cu_offset));
	  break;
	case DW_OP_call_ref:
	  /* FIXME: Strictly speaking for 64-bit DWARF3 files
	     this ought to be an 8-byte wide computation.  */
	  if (dwarf_version == -1)
	    {
	      printf (_("(DW_OP_call_ref in frame info)"));
	      /* No way to tell where the next op is, so just bail.  */
	      return need_frame_base;
	    }
	  if (dwarf_version == 2)
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
	    }
	  else
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, offset_size, end);
	    }
	  printf ("DW_OP_call_ref: <0x%s>", dwarf_vmatoa ("x", uvalue));
	  break;
	case DW_OP_form_tls_address:
	  printf ("DW_OP_form_tls_address");
	  break;
	case DW_OP_call_frame_cfa:
	  printf ("DW_OP_call_frame_cfa");
	  break;
	case DW_OP_bit_piece:
	  printf ("DW_OP_bit_piece: ");
	  READ_ULEB (uvalue, data, end);
	  printf (_("size: %s "), dwarf_vmatoa ("u", uvalue));
	  READ_ULEB (uvalue, data, end);
	  printf (_("offset: %s "), dwarf_vmatoa ("u", uvalue));
	  break;

	  /* DWARF 4 extensions.  */
	case DW_OP_stack_value:
	  printf ("DW_OP_stack_value");
	  break;

	case DW_OP_implicit_value:
	  printf ("DW_OP_implicit_value");
	  READ_ULEB (uvalue, data, end);
	  data = display_block (data, uvalue, end, ' ');
	  break;

	  /* GNU extensions.  */
	case DW_OP_GNU_push_tls_address:
	  printf (_("DW_OP_GNU_push_tls_address or DW_OP_HP_unknown"));
	  break;
	case DW_OP_GNU_uninit:
	  printf ("DW_OP_GNU_uninit");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_GNU_encoded_addr:
	  {
	    int encoding = 0;
	    dwarf_vma addr;

	    if (data < end)
	      encoding = *data++;
	    addr = get_encoded_value (&data, encoding, section, end);

	    printf ("DW_OP_GNU_encoded_addr: fmt:%02x addr:", encoding);
	    print_dwarf_vma (addr, pointer_size);
	  }
	  break;
	case DW_OP_implicit_pointer:
	case DW_OP_GNU_implicit_pointer:
	  /* FIXME: Strictly speaking for 64-bit DWARF3 files
	     this ought to be an 8-byte wide computation.  */
	  if (dwarf_version == -1)
	    {
	      printf (_("(%s in frame info)"),
		      (op == DW_OP_implicit_pointer
		       ? "DW_OP_implicit_pointer"
		       : "DW_OP_GNU_implicit_pointer"));
	      /* No way to tell where the next op is, so just bail.  */
	      return need_frame_base;
	    }
	  if (dwarf_version == 2)
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
	    }
	  else
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, offset_size, end);
	    }
	  READ_SLEB (svalue, data, end);
	  printf ("%s: <0x%s> %s",
		  (op == DW_OP_implicit_pointer
		   ? "DW_OP_implicit_pointer" : "DW_OP_GNU_implicit_pointer"),
		  dwarf_vmatoa ("x", uvalue),
		  dwarf_vmatoa ("d", svalue));
	  break;
	case DW_OP_entry_value:
	case DW_OP_GNU_entry_value:
	  READ_ULEB (uvalue, data, end);
	  /* PR 17531: file: 0cc9cd00.  */
	  if (uvalue > (dwarf_vma) (end - data))
	    uvalue = end - data;
	  printf ("%s: (", (op == DW_OP_entry_value ? "DW_OP_entry_value"
						    : "DW_OP_GNU_entry_value"));
	  if (decode_location_expression (data, pointer_size, offset_size,
					  dwarf_version, uvalue,
					  cu_offset, section))
	    need_frame_base = 1;
	  putchar (')');
	  data += uvalue;
	  break;
	case DW_OP_const_type:
	case DW_OP_GNU_const_type:
	  READ_ULEB (uvalue, data, end);
	  printf ("%s: <0x%s> ",
		  (op == DW_OP_const_type ? "DW_OP_const_type"
					  : "DW_OP_GNU_const_type"),
		  dwarf_vmatoa ("x", cu_offset + uvalue));
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  data = display_block (data, uvalue, end, ' ');
	  break;
	case DW_OP_regval_type:
	case DW_OP_GNU_regval_type:
	  READ_ULEB (uvalue, data, end);
	  printf ("%s: %s (%s)",
		  (op == DW_OP_regval_type ? "DW_OP_regval_type"
					   : "DW_OP_GNU_regval_type"),
		  dwarf_vmatoa ("u", uvalue), regname (uvalue, 1));
	  READ_ULEB (uvalue, data, end);
	  printf (" <0x%s>", dwarf_vmatoa ("x", cu_offset + uvalue));
	  break;
	case DW_OP_deref_type:
	case DW_OP_GNU_deref_type:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 1, end);
	  printf ("%s: %ld",
		  (op == DW_OP_deref_type ? "DW_OP_deref_type"
					  : "DW_OP_GNU_deref_type"),
		  (long) uvalue);
	  READ_ULEB (uvalue, data, end);
	  printf (" <0x%s>", dwarf_vmatoa ("x", cu_offset + uvalue));
	  break;
	case DW_OP_convert:
	case DW_OP_GNU_convert:
	  READ_ULEB (uvalue, data, end);
	  printf ("%s <0x%s>",
		  (op == DW_OP_convert ? "DW_OP_convert" : "DW_OP_GNU_convert"),
		  dwarf_vmatoa ("x", uvalue ? cu_offset + uvalue : 0));
	  break;
	case DW_OP_reinterpret:
	case DW_OP_GNU_reinterpret:
	  READ_ULEB (uvalue, data, end);
	  printf ("%s <0x%s>",
		  (op == DW_OP_reinterpret ? "DW_OP_reinterpret"
					   : "DW_OP_GNU_reinterpret"),
		  dwarf_vmatoa ("x", uvalue ? cu_offset + uvalue : 0));
	  break;
	case DW_OP_GNU_parameter_ref:
	  SAFE_BYTE_GET_AND_INC (uvalue, data, 4, end);
	  printf ("DW_OP_GNU_parameter_ref: <0x%s>",
		  dwarf_vmatoa ("x", cu_offset + uvalue));
	  break;
	case DW_OP_addrx:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_addrx <0x%s>", dwarf_vmatoa ("x", uvalue));
	  break;
	case DW_OP_GNU_addr_index:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_GNU_addr_index <0x%s>", dwarf_vmatoa ("x", uvalue));
	  break;
	case DW_OP_GNU_const_index:
	  READ_ULEB (uvalue, data, end);
	  printf ("DW_OP_GNU_const_index <0x%s>", dwarf_vmatoa ("x", uvalue));
	  break;
	case DW_OP_GNU_variable_value:
	  /* FIXME: Strictly speaking for 64-bit DWARF3 files
	     this ought to be an 8-byte wide computation.  */
	  if (dwarf_version == -1)
	    {
	      printf (_("(DW_OP_GNU_variable_value in frame info)"));
	      /* No way to tell where the next op is, so just bail.  */
	      return need_frame_base;
	    }
	  if (dwarf_version == 2)
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, pointer_size, end);
	    }
	  else
	    {
	      SAFE_BYTE_GET_AND_INC (uvalue, data, offset_size, end);
	    }
	  printf ("DW_OP_GNU_variable_value: <0x%s>", dwarf_vmatoa ("x", uvalue));
	  break;

	  /* HP extensions.  */
	case DW_OP_HP_is_value:
	  printf ("DW_OP_HP_is_value");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_HP_fltconst4:
	  printf ("DW_OP_HP_fltconst4");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_HP_fltconst8:
	  printf ("DW_OP_HP_fltconst8");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_HP_mod_range:
	  printf ("DW_OP_HP_mod_range");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_HP_unmod_range:
	  printf ("DW_OP_HP_unmod_range");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;
	case DW_OP_HP_tls:
	  printf ("DW_OP_HP_tls");
	  /* FIXME: Is there data associated with this OP ?  */
	  break;

	  /* PGI (STMicroelectronics) extensions.  */
	case DW_OP_PGI_omp_thread_num:
	  /* Pushes the thread number for the current thread as it would be
	     returned by the standard OpenMP library function:
	     omp_get_thread_num().  The "current thread" is the thread for
	     which the expression is being evaluated.  */
	  printf ("DW_OP_PGI_omp_thread_num");
	  break;

	default:
	  if (op >= DW_OP_lo_user
	      && op <= DW_OP_hi_user)
	    printf (_("(User defined location op 0x%x)"), op);
	  else
	    printf (_("(Unknown location op 0x%x)"), op);
	  /* No way to tell where the next op is, so just bail.  */
	  return need_frame_base;
	}

      /* Separate the ops.  */
      if (data < end)
	printf ("; ");
    }

  return need_frame_base;
}