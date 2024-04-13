display_debug_frames (struct dwarf_section *section,
		      void *file ATTRIBUTE_UNUSED)
{
  unsigned char *start = section->start;
  unsigned char *end = start + section->size;
  unsigned char *section_start = start;
  Frame_Chunk *chunks = NULL, *forward_refs = NULL;
  Frame_Chunk *remembered_state = NULL;
  Frame_Chunk *rs;
  bool is_eh = strcmp (section->name, ".eh_frame") == 0;
  unsigned int max_regs = 0;
  const char *bad_reg = _("bad register: ");
  unsigned int saved_eh_addr_size = eh_addr_size;

  introduce (section, false);

  while (start < end)
    {
      unsigned char *saved_start;
      unsigned char *block_end;
      dwarf_vma length;
      dwarf_vma cie_id;
      Frame_Chunk *fc;
      Frame_Chunk *cie;
      int need_col_headers = 1;
      unsigned char *augmentation_data = NULL;
      bfd_size_type augmentation_data_len = 0;
      unsigned int encoded_ptr_size = saved_eh_addr_size;
      unsigned int offset_size;
      bool all_nops;
      static Frame_Chunk fde_fc;

      saved_start = start;

      SAFE_BYTE_GET_AND_INC (length, start, 4, end);

      if (length == 0)
	{
	  printf ("\n%08lx ZERO terminator\n\n",
		    (unsigned long)(saved_start - section_start));
	  /* Skip any zero terminators that directly follow.
	     A corrupt section size could have loaded a whole
	     slew of zero filled memory bytes.  eg
	     PR 17512: file: 070-19381-0.004.  */
	  while (start < end && * start == 0)
	    ++ start;
	  continue;
	}

      if (length == 0xffffffff)
	{
	  SAFE_BYTE_GET_AND_INC (length, start, 8, end);
	  offset_size = 8;
	}
      else
	offset_size = 4;

      if (length > (size_t) (end - start))
	{
	  warn ("Invalid length 0x%s in FDE at %#08lx\n",
		dwarf_vmatoa_1 (NULL, length, offset_size),
		(unsigned long) (saved_start - section_start));
	  block_end = end;
	}
      else
	block_end = start + length;

      SAFE_BYTE_GET_AND_INC (cie_id, start, offset_size, block_end);

      if (is_eh ? (cie_id == 0) : ((offset_size == 4 && cie_id == DW_CIE_ID)
				   || (offset_size == 8 && cie_id == DW64_CIE_ID)))
	{
	  int version;
	  unsigned int mreg;

	  start = read_cie (start, block_end, &cie, &version,
			    &augmentation_data_len, &augmentation_data);
	  /* PR 17512: file: 027-135133-0.005.  */
	  if (cie == NULL)
	    break;

	  fc = cie;
	  fc->next = chunks;
	  chunks = fc;
	  fc->chunk_start = saved_start;
	  mreg = max_regs > 0 ? max_regs - 1 : 0;
	  if (mreg < fc->ra)
	    mreg = fc->ra;
	  if (frame_need_space (fc, mreg) < 0)
	    break;
	  if (fc->fde_encoding)
	    encoded_ptr_size = size_of_encoded_value (fc->fde_encoding);

	  printf ("\n%08lx ", (unsigned long) (saved_start - section_start));
	  print_dwarf_vma (length, fc->ptr_size);
	  print_dwarf_vma (cie_id, offset_size);

	  if (do_debug_frames_interp)
	    {
	      printf ("CIE \"%s\" cf=%d df=%d ra=%d\n", fc->augmentation,
		      fc->code_factor, fc->data_factor, fc->ra);
	    }
	  else
	    {
	      printf ("CIE\n");
	      printf ("  Version:               %d\n", version);
	      printf ("  Augmentation:          \"%s\"\n", fc->augmentation);
	      if (version >= 4)
		{
		  printf ("  Pointer Size:          %u\n", fc->ptr_size);
		  printf ("  Segment Size:          %u\n", fc->segment_size);
		}
	      printf ("  Code alignment factor: %u\n", fc->code_factor);
	      printf ("  Data alignment factor: %d\n", fc->data_factor);
	      printf ("  Return address column: %d\n", fc->ra);

	      if (augmentation_data_len)
		display_augmentation_data (augmentation_data, augmentation_data_len);

	      putchar ('\n');
	    }
	}
      else
	{
	  unsigned char *look_for;
	  unsigned long segment_selector;
	  dwarf_vma cie_off;

	  cie_off = cie_id;
	  if (is_eh)
	    {
	      dwarf_vma sign = (dwarf_vma) 1 << (offset_size * 8 - 1);
	      cie_off = (cie_off ^ sign) - sign;
	      cie_off = start - 4 - section_start - cie_off;
	    }

	  look_for = section_start + cie_off;
	  if (cie_off <= (dwarf_vma) (saved_start - section_start))
	    {
	      for (cie = chunks; cie ; cie = cie->next)
		if (cie->chunk_start == look_for)
		  break;
	    }
	  else if (cie_off >= section->size)
	    cie = NULL;
	  else
	    {
	      for (cie = forward_refs; cie ; cie = cie->next)
		if (cie->chunk_start == look_for)
		  break;
	      if (!cie)
		{
		  unsigned int off_size;
		  unsigned char *cie_scan;

		  cie_scan = look_for;
		  off_size = 4;
		  SAFE_BYTE_GET_AND_INC (length, cie_scan, 4, end);
		  if (length == 0xffffffff)
		    {
		      SAFE_BYTE_GET_AND_INC (length, cie_scan, 8, end);
		      off_size = 8;
		    }
		  if (length != 0 && length <= (size_t) (end - cie_scan))
		    {
		      dwarf_vma c_id;
		      unsigned char *cie_end = cie_scan + length;

		      SAFE_BYTE_GET_AND_INC (c_id, cie_scan, off_size,
					     cie_end);
		      if (is_eh
			  ? c_id == 0
			  : ((off_size == 4 && c_id == DW_CIE_ID)
			     || (off_size == 8 && c_id == DW64_CIE_ID)))
			{
			  int version;
			  unsigned int mreg;

			  read_cie (cie_scan, cie_end, &cie, &version,
				    &augmentation_data_len, &augmentation_data);
			  /* PR 17512: file: 3450-2098-0.004.  */
			  if (cie == NULL)
			    {
			      warn (_("Failed to read CIE information\n"));
			      break;
			    }
			  cie->next = forward_refs;
			  forward_refs = cie;
			  cie->chunk_start = look_for;
			  mreg = max_regs > 0 ? max_regs - 1 : 0;
			  if (mreg < cie->ra)
			    mreg = cie->ra;
			  if (frame_need_space (cie, mreg) < 0)
			    {
			      warn (_("Invalid max register\n"));
			      break;
			    }
			  if (cie->fde_encoding)
			    encoded_ptr_size
			      = size_of_encoded_value (cie->fde_encoding);
			}
		    }
		}
	    }

	  fc = &fde_fc;
	  memset (fc, 0, sizeof (Frame_Chunk));

	  if (!cie)
	    {
	      fc->ncols = 0;
	      fc->col_type = (short int *) xmalloc (sizeof (short int));
	      fc->col_offset = (int *) xmalloc (sizeof (int));
	      if (frame_need_space (fc, max_regs > 0 ? max_regs - 1 : 0) < 0)
		{
		  warn (_("Invalid max register\n"));
		  break;
		}
	      cie = fc;
	      fc->augmentation = "";
	      fc->fde_encoding = 0;
	      fc->ptr_size = eh_addr_size;
	      fc->segment_size = 0;
	    }
	  else
	    {
	      fc->ncols = cie->ncols;
	      fc->col_type = (short int *) xcmalloc (fc->ncols, sizeof (short int));
	      fc->col_offset =  (int *) xcmalloc (fc->ncols, sizeof (int));
	      memcpy (fc->col_type, cie->col_type, fc->ncols * sizeof (short int));
	      memcpy (fc->col_offset, cie->col_offset, fc->ncols * sizeof (int));
	      fc->augmentation = cie->augmentation;
	      fc->ptr_size = cie->ptr_size;
	      eh_addr_size = cie->ptr_size;
	      fc->segment_size = cie->segment_size;
	      fc->code_factor = cie->code_factor;
	      fc->data_factor = cie->data_factor;
	      fc->cfa_reg = cie->cfa_reg;
	      fc->cfa_offset = cie->cfa_offset;
	      fc->ra = cie->ra;
	      if (frame_need_space (fc, max_regs > 0 ? max_regs - 1: 0) < 0)
		{
		  warn (_("Invalid max register\n"));
		  break;
		}
	      fc->fde_encoding = cie->fde_encoding;
	    }

	  if (fc->fde_encoding)
	    encoded_ptr_size = size_of_encoded_value (fc->fde_encoding);

	  segment_selector = 0;
	  if (fc->segment_size)
	    {
	      if (fc->segment_size > sizeof (segment_selector))
		{
		  /* PR 17512: file: 9e196b3e.  */
		  warn (_("Probably corrupt segment size: %d - using 4 instead\n"), fc->segment_size);
		  fc->segment_size = 4;
		}
	      SAFE_BYTE_GET_AND_INC (segment_selector, start,
				     fc->segment_size, block_end);
	    }

	  fc->pc_begin = get_encoded_value (&start, fc->fde_encoding, section,
					    block_end);

	  /* FIXME: It appears that sometimes the final pc_range value is
	     encoded in less than encoded_ptr_size bytes.  See the x86_64
	     run of the "objcopy on compressed debug sections" test for an
	     example of this.  */
	  SAFE_BYTE_GET_AND_INC (fc->pc_range, start, encoded_ptr_size,
				 block_end);

	  if (cie->augmentation[0] == 'z')
	    {
	      READ_ULEB (augmentation_data_len, start, block_end);
	      augmentation_data = start;
	      /* PR 17512 file: 722-8446-0.004 and PR 22386.  */
	      if (augmentation_data_len > (bfd_size_type) (block_end - start))
		{
		  warn (_("Augmentation data too long: 0x%s, "
			  "expected at most %#lx\n"),
			dwarf_vmatoa ("x", augmentation_data_len),
			(unsigned long) (block_end - start));
		  start = block_end;
		  augmentation_data = NULL;
		  augmentation_data_len = 0;
		}
	      start += augmentation_data_len;
	    }

	  printf ("\n%08lx %s %s FDE ",
		  (unsigned long)(saved_start - section_start),
		  dwarf_vmatoa_1 (NULL, length, fc->ptr_size),
		  dwarf_vmatoa_1 (NULL, cie_id, offset_size));

	  if (cie->chunk_start)
	    printf ("cie=%08lx",
		    (unsigned long) (cie->chunk_start - section_start));
	  else
	    /* Ideally translate "invalid " to 8 chars, trailing space
	       is optional.  */
	    printf (_("cie=invalid "));

	  printf (" pc=");
	  if (fc->segment_size)
	    printf ("%04lx:", segment_selector);

	  printf ("%s..%s\n",
		  dwarf_vmatoa_1 (NULL, fc->pc_begin, fc->ptr_size),
		  dwarf_vmatoa_1 (NULL, fc->pc_begin + fc->pc_range, fc->ptr_size));

	  if (! do_debug_frames_interp && augmentation_data_len)
	    {
	      display_augmentation_data (augmentation_data, augmentation_data_len);
	      putchar ('\n');
	    }
	}

      /* At this point, fc is the current chunk, cie (if any) is set, and
	 we're about to interpret instructions for the chunk.  */
      /* ??? At present we need to do this always, since this sizes the
	 fc->col_type and fc->col_offset arrays, which we write into always.
	 We should probably split the interpreted and non-interpreted bits
	 into two different routines, since there's so much that doesn't
	 really overlap between them.  */
      if (1 || do_debug_frames_interp)
	{
	  /* Start by making a pass over the chunk, allocating storage
	     and taking note of what registers are used.  */
	  unsigned char *tmp = start;

	  while (start < block_end)
	    {
	      unsigned int reg, op, opa;
	      unsigned long temp;

	      op = *start++;
	      opa = op & 0x3f;
	      if (op & 0xc0)
		op &= 0xc0;

	      /* Warning: if you add any more cases to this switch, be
		 sure to add them to the corresponding switch below.  */
	      reg = -1u;
	      switch (op)
		{
		case DW_CFA_advance_loc:
		  break;
		case DW_CFA_offset:
		  SKIP_ULEB (start, block_end);
		  reg = opa;
		  break;
		case DW_CFA_restore:
		  reg = opa;
		  break;
		case DW_CFA_set_loc:
		  if ((size_t) (block_end - start) < encoded_ptr_size)
		    start = block_end;
		  else
		    start += encoded_ptr_size;
		  break;
		case DW_CFA_advance_loc1:
		  if ((size_t) (block_end - start) < 1)
		    start = block_end;
		  else
		    start += 1;
		  break;
		case DW_CFA_advance_loc2:
		  if ((size_t) (block_end - start) < 2)
		    start = block_end;
		  else
		    start += 2;
		  break;
		case DW_CFA_advance_loc4:
		  if ((size_t) (block_end - start) < 4)
		    start = block_end;
		  else
		    start += 4;
		  break;
		case DW_CFA_offset_extended:
		case DW_CFA_val_offset:
		  READ_ULEB (reg, start, block_end);
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_restore_extended:
		  READ_ULEB (reg, start, block_end);
		  break;
		case DW_CFA_undefined:
		  READ_ULEB (reg, start, block_end);
		  break;
		case DW_CFA_same_value:
		  READ_ULEB (reg, start, block_end);
		  break;
		case DW_CFA_register:
		  READ_ULEB (reg, start, block_end);
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_def_cfa:
		  SKIP_ULEB (start, block_end);
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_def_cfa_register:
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_def_cfa_offset:
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_def_cfa_expression:
		  READ_ULEB (temp, start, block_end);
		  if ((size_t) (block_end - start) < temp)
		    start = block_end;
		  else
		    start += temp;
		  break;
		case DW_CFA_expression:
		case DW_CFA_val_expression:
		  READ_ULEB (reg, start, block_end);
		  READ_ULEB (temp, start, block_end);
		  if ((size_t) (block_end - start) < temp)
		    start = block_end;
		  else
		    start += temp;
		  break;
		case DW_CFA_offset_extended_sf:
		case DW_CFA_val_offset_sf:
		  READ_ULEB (reg, start, block_end);
		  SKIP_SLEB (start, block_end);
		  break;
		case DW_CFA_def_cfa_sf:
		  SKIP_ULEB (start, block_end);
		  SKIP_SLEB (start, block_end);
		  break;
		case DW_CFA_def_cfa_offset_sf:
		  SKIP_SLEB (start, block_end);
		  break;
		case DW_CFA_MIPS_advance_loc8:
		  if ((size_t) (block_end - start) < 8)
		    start = block_end;
		  else
		    start += 8;
		  break;
		case DW_CFA_GNU_args_size:
		  SKIP_ULEB (start, block_end);
		  break;
		case DW_CFA_GNU_negative_offset_extended:
		  READ_ULEB (reg, start, block_end);
		  SKIP_ULEB (start, block_end);
		  break;
		default:
		  break;
		}
	      if (reg != -1u && frame_need_space (fc, reg) >= 0)
		{
		  /* Don't leave any reg as DW_CFA_unreferenced so
		     that frame_display_row prints name of regs in
		     header, and all referenced regs in each line.  */
		  if (reg >= cie->ncols
		      || cie->col_type[reg] == DW_CFA_unreferenced)
		    fc->col_type[reg] = DW_CFA_undefined;
		  else
		    fc->col_type[reg] = cie->col_type[reg];
		}
	    }
	  start = tmp;
	}

      all_nops = true;

      /* Now we know what registers are used, make a second pass over
	 the chunk, this time actually printing out the info.  */

      while (start < block_end)
	{
	  unsigned op, opa;
	  unsigned long ul, roffs;
	  /* Note: It is tempting to use an unsigned long for 'reg' but there
	     are various functions, notably frame_space_needed() that assume that
	     reg is an unsigned int.  */
	  unsigned int reg;
	  dwarf_signed_vma l;
	  dwarf_vma ofs;
	  dwarf_vma vma;
	  const char *reg_prefix = "";

	  op = *start++;
	  opa = op & 0x3f;
	  if (op & 0xc0)
	    op &= 0xc0;

	  /* Make a note if something other than DW_CFA_nop happens.  */
	  if (op != DW_CFA_nop)
	    all_nops = false;

	  /* Warning: if you add any more cases to this switch, be
	     sure to add them to the corresponding switch above.  */
	  switch (op)
	    {
	    case DW_CFA_advance_loc:
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_advance_loc: %d to %s\n",
			opa * fc->code_factor,
			dwarf_vmatoa_1 (NULL,
					fc->pc_begin + opa * fc->code_factor,
					fc->ptr_size));
	      fc->pc_begin += opa * fc->code_factor;
	      break;

	    case DW_CFA_offset:
	      READ_ULEB (roffs, start, block_end);
	      if (opa >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_offset: %s%s at cfa%+ld\n",
			reg_prefix, regname (opa, 0),
			roffs * fc->data_factor);
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[opa] = DW_CFA_offset;
		  fc->col_offset[opa] = roffs * fc->data_factor;
		}
	      break;

	    case DW_CFA_restore:
	      if (opa >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_restore: %s%s\n",
			reg_prefix, regname (opa, 0));
	      if (*reg_prefix != '\0')
		break;

	      if (opa >= cie->ncols
		  || cie->col_type[opa] == DW_CFA_unreferenced)
		{
		  fc->col_type[opa] = DW_CFA_undefined;
		  fc->col_offset[opa] = 0;
		}
	      else
		{
		  fc->col_type[opa] = cie->col_type[opa];
		  fc->col_offset[opa] = cie->col_offset[opa];
		}
	      break;

	    case DW_CFA_set_loc:
	      vma = get_encoded_value (&start, fc->fde_encoding, section,
				       block_end);
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_set_loc: %s\n",
			dwarf_vmatoa_1 (NULL, vma, fc->ptr_size));
	      fc->pc_begin = vma;
	      break;

	    case DW_CFA_advance_loc1:
	      SAFE_BYTE_GET_AND_INC (ofs, start, 1, block_end);
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_advance_loc1: %ld to %s\n",
			(unsigned long) (ofs * fc->code_factor),
			dwarf_vmatoa_1 (NULL,
					fc->pc_begin + ofs * fc->code_factor,
					fc->ptr_size));
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_advance_loc2:
	      SAFE_BYTE_GET_AND_INC (ofs, start, 2, block_end);
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_advance_loc2: %ld to %s\n",
			(unsigned long) (ofs * fc->code_factor),
			dwarf_vmatoa_1 (NULL,
					fc->pc_begin + ofs * fc->code_factor,
					fc->ptr_size));
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_advance_loc4:
	      SAFE_BYTE_GET_AND_INC (ofs, start, 4, block_end);
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_advance_loc4: %ld to %s\n",
			(unsigned long) (ofs * fc->code_factor),
			dwarf_vmatoa_1 (NULL,
					fc->pc_begin + ofs * fc->code_factor,
					fc->ptr_size));
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_offset_extended:
	      READ_ULEB (reg, start, block_end);
	      READ_ULEB (roffs, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_offset_extended: %s%s at cfa%+ld\n",
			reg_prefix, regname (reg, 0),
			roffs * fc->data_factor);
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_offset;
		  fc->col_offset[reg] = roffs * fc->data_factor;
		}
	      break;

	    case DW_CFA_val_offset:
	      READ_ULEB (reg, start, block_end);
	      READ_ULEB (roffs, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_val_offset: %s%s is cfa%+ld\n",
			reg_prefix, regname (reg, 0),
			roffs * fc->data_factor);
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_val_offset;
		  fc->col_offset[reg] = roffs * fc->data_factor;
		}
	      break;

	    case DW_CFA_restore_extended:
	      READ_ULEB (reg, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_restore_extended: %s%s\n",
			reg_prefix, regname (reg, 0));
	      if (*reg_prefix != '\0')
		break;

	      if (reg >= cie->ncols
		  || cie->col_type[reg] == DW_CFA_unreferenced)
		{
		  fc->col_type[reg] = DW_CFA_undefined;
		  fc->col_offset[reg] = 0;
		}
	      else
		{
		  fc->col_type[reg] = cie->col_type[reg];
		  fc->col_offset[reg] = cie->col_offset[reg];
		}
	      break;

	    case DW_CFA_undefined:
	      READ_ULEB (reg, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_undefined: %s%s\n",
			reg_prefix, regname (reg, 0));
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_undefined;
		  fc->col_offset[reg] = 0;
		}
	      break;

	    case DW_CFA_same_value:
	      READ_ULEB (reg, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_same_value: %s%s\n",
			reg_prefix, regname (reg, 0));
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_same_value;
		  fc->col_offset[reg] = 0;
		}
	      break;

	    case DW_CFA_register:
	      READ_ULEB (reg, start, block_end);
	      READ_ULEB (roffs, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		{
		  printf ("  DW_CFA_register: %s%s in ",
			  reg_prefix, regname (reg, 0));
		  puts (regname (roffs, 0));
		}
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_register;
		  fc->col_offset[reg] = roffs;
		}
	      break;

	    case DW_CFA_remember_state:
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_remember_state\n");
	      rs = (Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
	      rs->cfa_offset = fc->cfa_offset;
	      rs->cfa_reg = fc->cfa_reg;
	      rs->ra = fc->ra;
	      rs->cfa_exp = fc->cfa_exp;
	      rs->ncols = fc->ncols;
	      rs->col_type = (short int *) xcmalloc (rs->ncols,
						     sizeof (* rs->col_type));
	      rs->col_offset = (int *) xcmalloc (rs->ncols, sizeof (* rs->col_offset));
	      memcpy (rs->col_type, fc->col_type, rs->ncols * sizeof (* fc->col_type));
	      memcpy (rs->col_offset, fc->col_offset, rs->ncols * sizeof (* fc->col_offset));
	      rs->next = remembered_state;
	      remembered_state = rs;
	      break;

	    case DW_CFA_restore_state:
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_restore_state\n");
	      rs = remembered_state;
	      if (rs)
		{
		  remembered_state = rs->next;
		  fc->cfa_offset = rs->cfa_offset;
		  fc->cfa_reg = rs->cfa_reg;
		  fc->ra = rs->ra;
		  fc->cfa_exp = rs->cfa_exp;
		  if (frame_need_space (fc, rs->ncols - 1) < 0)
		    {
		      warn (_("Invalid column number in saved frame state\n"));
		      fc->ncols = 0;
		      break;
		    }
		  memcpy (fc->col_type, rs->col_type, rs->ncols * sizeof (* rs->col_type));
		  memcpy (fc->col_offset, rs->col_offset,
			  rs->ncols * sizeof (* rs->col_offset));
		  free (rs->col_type);
		  free (rs->col_offset);
		  free (rs);
		}
	      else if (do_debug_frames_interp)
		printf ("Mismatched DW_CFA_restore_state\n");
	      break;

	    case DW_CFA_def_cfa:
	      READ_ULEB (fc->cfa_reg, start, block_end);
	      READ_ULEB (fc->cfa_offset, start, block_end);
	      fc->cfa_exp = 0;
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_def_cfa: %s ofs %d\n",
			regname (fc->cfa_reg, 0), (int) fc->cfa_offset);
	      break;

	    case DW_CFA_def_cfa_register:
	      READ_ULEB (fc->cfa_reg, start, block_end);
	      fc->cfa_exp = 0;
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_def_cfa_register: %s\n",
			regname (fc->cfa_reg, 0));
	      break;

	    case DW_CFA_def_cfa_offset:
	      READ_ULEB (fc->cfa_offset, start, block_end);
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_def_cfa_offset: %d\n", (int) fc->cfa_offset);
	      break;

	    case DW_CFA_nop:
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_nop\n");
	      break;

	    case DW_CFA_def_cfa_expression:
	      READ_ULEB (ul, start, block_end);
	      if (ul > (size_t) (block_end - start))
		{
		  printf (_("  DW_CFA_def_cfa_expression: <corrupt len %lu>\n"), ul);
		  break;
		}
	      if (! do_debug_frames_interp)
		{
		  printf ("  DW_CFA_def_cfa_expression (");
		  decode_location_expression (start, eh_addr_size, 0, -1,
					      ul, 0, section);
		  printf (")\n");
		}
	      fc->cfa_exp = 1;
	      start += ul;
	      break;

	    case DW_CFA_expression:
	      READ_ULEB (reg, start, block_end);
	      READ_ULEB (ul, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      /* PR 17512: file: 069-133014-0.006.  */
	      /* PR 17512: file: 98c02eb4.  */
	      if (ul > (size_t) (block_end - start))
		{
		  printf (_("  DW_CFA_expression: <corrupt len %lu>\n"), ul);
		  break;
		}
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		{
		  printf ("  DW_CFA_expression: %s%s (",
			  reg_prefix, regname (reg, 0));
		  decode_location_expression (start, eh_addr_size, 0, -1,
					      ul, 0, section);
		  printf (")\n");
		}
	      if (*reg_prefix == '\0')
		fc->col_type[reg] = DW_CFA_expression;
	      start += ul;
	      break;

	    case DW_CFA_val_expression:
	      READ_ULEB (reg, start, block_end);
	      READ_ULEB (ul, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (ul > (size_t) (block_end - start))
		{
		  printf ("  DW_CFA_val_expression: <corrupt len %lu>\n", ul);
		  break;
		}
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		{
		  printf ("  DW_CFA_val_expression: %s%s (",
			  reg_prefix, regname (reg, 0));
		  decode_location_expression (start, eh_addr_size, 0, -1,
					      ul, 0, section);
		  printf (")\n");
		}
	      if (*reg_prefix == '\0')
		fc->col_type[reg] = DW_CFA_val_expression;
	      start += ul;
	      break;

	    case DW_CFA_offset_extended_sf:
	      READ_ULEB (reg, start, block_end);
	      READ_SLEB (l, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_offset_extended_sf: %s%s at cfa%+ld\n",
			reg_prefix, regname (reg, 0),
			(long)(l * fc->data_factor));
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_offset;
		  fc->col_offset[reg] = l * fc->data_factor;
		}
	      break;

	    case DW_CFA_val_offset_sf:
	      READ_ULEB (reg, start, block_end);
	      READ_SLEB (l, start, block_end);
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_val_offset_sf: %s%s is cfa%+ld\n",
			reg_prefix, regname (reg, 0),
			(long)(l * fc->data_factor));
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_val_offset;
		  fc->col_offset[reg] = l * fc->data_factor;
		}
	      break;

	    case DW_CFA_def_cfa_sf:
	      READ_ULEB (fc->cfa_reg, start, block_end);
	      READ_SLEB (l, start, block_end);
	      l *= fc->data_factor;
	      fc->cfa_offset = l;
	      fc->cfa_exp = 0;
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_def_cfa_sf: %s ofs %ld\n",
			regname (fc->cfa_reg, 0), (long) l);
	      break;

	    case DW_CFA_def_cfa_offset_sf:
	      READ_SLEB (l, start, block_end);
	      l *= fc->data_factor;
	      fc->cfa_offset = l;
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_def_cfa_offset_sf: %ld\n", (long) l);
	      break;

	    case DW_CFA_MIPS_advance_loc8:
	      SAFE_BYTE_GET_AND_INC (ofs, start, 8, block_end);
	      if (do_debug_frames_interp)
		frame_display_row (fc, &need_col_headers, &max_regs);
	      else
		printf ("  DW_CFA_MIPS_advance_loc8: %ld to %s\n",
			(unsigned long) (ofs * fc->code_factor),
			dwarf_vmatoa_1 (NULL,
					fc->pc_begin + ofs * fc->code_factor,
					fc->ptr_size));
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_GNU_window_save:
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_GNU_window_save\n");
	      break;

	    case DW_CFA_GNU_args_size:
	      READ_ULEB (ul, start, block_end);
	      if (! do_debug_frames_interp)
		printf ("  DW_CFA_GNU_args_size: %ld\n", ul);
	      break;

	    case DW_CFA_GNU_negative_offset_extended:
	      READ_ULEB (reg, start, block_end);
	      READ_SLEB (l, start, block_end);
	      l = - l;
	      if (reg >= fc->ncols)
		reg_prefix = bad_reg;
	      if (! do_debug_frames_interp || *reg_prefix != '\0')
		printf ("  DW_CFA_GNU_negative_offset_extended: %s%s at cfa%+ld\n",
			reg_prefix, regname (reg, 0),
			(long)(l * fc->data_factor));
	      if (*reg_prefix == '\0')
		{
		  fc->col_type[reg] = DW_CFA_offset;
		  fc->col_offset[reg] = l * fc->data_factor;
		}
	      break;

	    default:
	      if (op >= DW_CFA_lo_user && op <= DW_CFA_hi_user)
		printf (_("  DW_CFA_??? (User defined call frame op: %#x)\n"), op);
	      else
		warn (_("Unsupported or unknown Dwarf Call Frame Instruction number: %#x\n"), op);
	      start = block_end;
	    }
	}

      /* Interpret the CFA - as long as it is not completely full of NOPs.  */
      if (do_debug_frames_interp && ! all_nops)
	frame_display_row (fc, &need_col_headers, &max_regs);

      if (fde_fc.col_type != NULL)
	{
	  free (fde_fc.col_type);
	  fde_fc.col_type = NULL;
	}
      if (fde_fc.col_offset != NULL)
	{
	  free (fde_fc.col_offset);
	  fde_fc.col_offset = NULL;
	}

      start = block_end;
      eh_addr_size = saved_eh_addr_size;
    }

  printf ("\n");

  while (remembered_state != NULL)
    {
      rs = remembered_state;
      remembered_state = rs->next;
      free (rs->col_type);
      free (rs->col_offset);
      rs->next = NULL; /* Paranoia.  */
      free (rs);
    }

  while (chunks != NULL)
    {
      rs = chunks;
      chunks = rs->next;
      free (rs->col_type);
      free (rs->col_offset);
      rs->next = NULL; /* Paranoia.  */
      free (rs);
    }

  while (forward_refs != NULL)
    {
      rs = forward_refs;
      forward_refs = rs->next;
      free (rs->col_type);
      free (rs->col_offset);
      rs->next = NULL; /* Paranoia.  */
      free (rs);
    }

  return 1;
}