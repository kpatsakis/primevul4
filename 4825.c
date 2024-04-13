read_debug_line_header (struct dwarf_section * section,
			unsigned char * data,
			unsigned char * end,
			DWARF2_Internal_LineInfo * linfo,
			unsigned char ** end_of_sequence)
{
  unsigned char *hdrptr;

  /* Extract information from the Line Number Program Header.
     (section 6.2.4 in the Dwarf3 doc).  */
  hdrptr = data;

  /* Get and check the length of the block.  */
  SAFE_BYTE_GET_AND_INC (linfo->li_length, hdrptr, 4, end);

  if (linfo->li_length == 0xffffffff)
    {
      /* This section is 64-bit DWARF 3.  */
      SAFE_BYTE_GET_AND_INC (linfo->li_length, hdrptr, 8, end);
      linfo->li_offset_size = 8;
    }
  else
    linfo->li_offset_size = 4;

  if (linfo->li_length > (size_t) (end - hdrptr))
    {
      /* If the length field has a relocation against it, then we should
	 not complain if it is inaccurate (and probably negative).  This
	 happens in object files when the .debug_line section is actually
	 comprised of several different .debug_line.* sections, (some of
	 which may be removed by linker garbage collection), and a relocation
	 is used to compute the correct length once that is done.  */
      if (reloc_at (section, (hdrptr - section->start) - linfo->li_offset_size))
	{
	  linfo->li_length = end - hdrptr;
	}
      else
	{
	  warn (_("The length field (0x%lx) in the debug_line header is wrong - the section is too small\n"),
		(long) linfo->li_length);
	  return NULL;
	}
    }
  end = hdrptr + linfo->li_length;

  /* Get and check the version number.  */
  SAFE_BYTE_GET_AND_INC (linfo->li_version, hdrptr, 2, end);

  if (linfo->li_version != 2
      && linfo->li_version != 3
      && linfo->li_version != 4
      && linfo->li_version != 5)
    {
      warn (_("Only DWARF version 2, 3, 4 and 5 line info "
	      "is currently supported.\n"));
      return NULL;
    }

  if (linfo->li_version >= 5)
    {
      SAFE_BYTE_GET_AND_INC (linfo->li_address_size, hdrptr, 1, end);

      SAFE_BYTE_GET_AND_INC (linfo->li_segment_size, hdrptr, 1, end);
      if (linfo->li_segment_size != 0)
	{
	  warn (_("The %s section contains "
		  "unsupported segment selector size: %d.\n"),
		section->name, linfo->li_segment_size);
	  return NULL;
	}
    }

  SAFE_BYTE_GET_AND_INC (linfo->li_prologue_length, hdrptr,
			 linfo->li_offset_size, end);
  SAFE_BYTE_GET_AND_INC (linfo->li_min_insn_length, hdrptr, 1, end);

  if (linfo->li_version >= 4)
    {
      SAFE_BYTE_GET_AND_INC (linfo->li_max_ops_per_insn, hdrptr, 1, end);

      if (linfo->li_max_ops_per_insn == 0)
	{
	  warn (_("Invalid maximum operations per insn.\n"));
	  return NULL;
	}
    }
  else
    linfo->li_max_ops_per_insn = 1;

  SAFE_BYTE_GET_AND_INC (linfo->li_default_is_stmt, hdrptr, 1, end);
  SAFE_SIGNED_BYTE_GET_AND_INC (linfo->li_line_base, hdrptr, 1, end);
  SAFE_BYTE_GET_AND_INC (linfo->li_line_range, hdrptr, 1, end);
  SAFE_BYTE_GET_AND_INC (linfo->li_opcode_base, hdrptr, 1, end);

  *end_of_sequence = end;
  return hdrptr;
}