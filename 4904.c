frame_need_space (Frame_Chunk *fc, unsigned int reg)
{
  unsigned int prev = fc->ncols;

  if (reg < (unsigned int) fc->ncols)
    return 0;

  if (dwarf_regnames_count > 0
      && reg > dwarf_regnames_count)
    return -1;

  fc->ncols = reg + 1;
  /* PR 17512: file: 10450-2643-0.004.
     If reg == -1 then this can happen...  */
  if (fc->ncols == 0)
    return -1;

  /* PR 17512: file: 2844a11d.  */
  if (fc->ncols > 1024 && dwarf_regnames_count == 0)
    {
      error (_("Unfeasibly large register number: %u\n"), reg);
      fc->ncols = 0;
      /* FIXME: 1024 is an arbitrary limit.  Increase it if
	 we ever encounter a valid binary that exceeds it.  */
      return -1;
    }

  fc->col_type = (short int *) xcrealloc (fc->col_type, fc->ncols,
					  sizeof (short int));
  fc->col_offset = (int *) xcrealloc (fc->col_offset, fc->ncols, sizeof (int));
  /* PR 17512: file:002-10025-0.005.  */
  if (fc->col_type == NULL || fc->col_offset == NULL)
    {
      error (_("Out of memory allocating %u columns in dwarf frame arrays\n"),
	     fc->ncols);
      fc->ncols = 0;
      return -1;
    }

  while (prev < fc->ncols)
    {
      fc->col_type[prev] = DW_CFA_unreferenced;
      fc->col_offset[prev] = 0;
      prev++;
    }
  return 1;
}