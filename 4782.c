get_line_filename_and_dirname (dwarf_vma line_offset,
			       dwarf_vma fileidx,
			       unsigned char **dir_name)
{
  struct dwarf_section *section = &debug_displays [line].section;
  unsigned char *hdrptr, *dirtable, *file_name;
  unsigned int offset_size;
  unsigned int version, opcode_base;
  dwarf_vma length, diridx;
  const unsigned char * end;

  *dir_name = NULL;
  if (section->start == NULL
      || line_offset >= section->size
      || fileidx == 0)
    return NULL;

  hdrptr = section->start + line_offset;
  end = section->start + section->size;

  SAFE_BYTE_GET_AND_INC (length, hdrptr, 4, end);
  if (length == 0xffffffff)
    {
      /* This section is 64-bit DWARF 3.  */
      SAFE_BYTE_GET_AND_INC (length, hdrptr, 8, end);
      offset_size = 8;
    }
  else
    offset_size = 4;

  if (length > (size_t) (end - hdrptr)
      || length < 2 + offset_size + 1 + 3 + 1)
    return NULL;
  end = hdrptr + length;

  SAFE_BYTE_GET_AND_INC (version, hdrptr, 2, end);
  if (version != 2 && version != 3 && version != 4)
    return NULL;
  hdrptr += offset_size + 1;/* Skip prologue_length and min_insn_length.  */
  if (version >= 4)
    hdrptr++;		    /* Skip max_ops_per_insn.  */
  hdrptr += 3;		    /* Skip default_is_stmt, line_base, line_range.  */

  SAFE_BYTE_GET_AND_INC (opcode_base, hdrptr, 1, end);
  if (opcode_base == 0
      || opcode_base - 1 >= (size_t) (end - hdrptr))
    return NULL;

  hdrptr += opcode_base - 1;

  dirtable = hdrptr;
  /* Skip over dirname table.  */
  while (*hdrptr != '\0')
    {
      hdrptr += strnlen ((char *) hdrptr, end - hdrptr);
      if (hdrptr < end)
	hdrptr++;
      if (hdrptr >= end)
	return NULL;
    }
  hdrptr++;		    /* Skip the NUL at the end of the table.  */

  /* Now skip over preceding filename table entries.  */
  for (; hdrptr < end && *hdrptr != '\0' && fileidx > 1; fileidx--)
    {
      hdrptr += strnlen ((char *) hdrptr, end - hdrptr);
      if (hdrptr < end)
	hdrptr++;
      SKIP_ULEB (hdrptr, end);
      SKIP_ULEB (hdrptr, end);
      SKIP_ULEB (hdrptr, end);
    }
  if (hdrptr >= end || *hdrptr == '\0')
    return NULL;

  file_name = hdrptr;
  hdrptr += strnlen ((char *) hdrptr, end - hdrptr);
  if (hdrptr < end)
    hdrptr++;
  if (hdrptr >= end)
    return NULL;
  READ_ULEB (diridx, hdrptr, end);
  if (diridx == 0)
    return file_name;
  for (; dirtable < end && *dirtable != '\0' && diridx > 1; diridx--)
    {
      dirtable += strnlen ((char *) dirtable, end - dirtable);
      if (dirtable < end)
	dirtable++;
    }
  if (dirtable >= end || *dirtable == '\0')
    return NULL;
  *dir_name = dirtable;
  return file_name;
}