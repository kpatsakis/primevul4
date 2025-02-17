read_cie (unsigned char *start, unsigned char *end,
	  Frame_Chunk **p_cie, int *p_version,
	  bfd_size_type *p_aug_len, unsigned char **p_aug)
{
  int version;
  Frame_Chunk *fc;
  unsigned char *augmentation_data = NULL;
  bfd_size_type augmentation_data_len = 0;

  * p_cie = NULL;
  /* PR 17512: file: 001-228113-0.004.  */
  if (start >= end)
    return end;

  fc = (Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
  memset (fc, 0, sizeof (Frame_Chunk));

  fc->col_type = (short int *) xmalloc (sizeof (short int));
  fc->col_offset = (int *) xmalloc (sizeof (int));

  version = *start++;

  fc->augmentation = (char *) start;
  /* PR 17512: file: 001-228113-0.004.
     Skip past augmentation name, but avoid running off the end of the data.  */
  while (start < end)
    if (* start ++ == '\0')
      break;
  if (start == end)
    {
      warn (_("No terminator for augmentation name\n"));
      goto fail;
    }

  if (strcmp (fc->augmentation, "eh") == 0)
    {
      if (eh_addr_size > (size_t) (end - start))
	goto fail;
      start += eh_addr_size;
    }

  if (version >= 4)
    {
      if (2 > (size_t) (end - start))
	goto fail;
      GET (fc->ptr_size, 1);
      if (fc->ptr_size < 1 || fc->ptr_size > 8)
	{
	  warn (_("Invalid pointer size (%d) in CIE data\n"), fc->ptr_size);
	  goto fail;
	}

      GET (fc->segment_size, 1);
      /* PR 17512: file: e99d2804.  */
      if (fc->segment_size > 8 || fc->segment_size + fc->ptr_size > 8)
	{
	  warn (_("Invalid segment size (%d) in CIE data\n"), fc->segment_size);
	  goto fail;
	}

      eh_addr_size = fc->ptr_size;
    }
  else
    {
      fc->ptr_size = eh_addr_size;
      fc->segment_size = 0;
    }

  READ_ULEB (fc->code_factor, start, end);
  READ_SLEB (fc->data_factor, start, end);

  if (start >= end)
    goto fail;

  if (version == 1)
    {
      GET (fc->ra, 1);
    }
  else
    {
      READ_ULEB (fc->ra, start, end);
    }

  if (fc->augmentation[0] == 'z')
    {
      if (start >= end)
	goto fail;
      READ_ULEB (augmentation_data_len, start, end);
      augmentation_data = start;
      /* PR 17512: file: 11042-2589-0.004.  */
      if (augmentation_data_len > (bfd_size_type) (end - start))
	{
	  warn (_("Augmentation data too long: 0x%s, expected at most %#lx\n"),
		dwarf_vmatoa ("x", augmentation_data_len),
		(unsigned long) (end - start));
	  goto fail;
	}
      start += augmentation_data_len;
    }

  if (augmentation_data_len)
    {
      unsigned char *p;
      unsigned char *q;
      unsigned char *qend;

      p = (unsigned char *) fc->augmentation + 1;
      q = augmentation_data;
      qend = q + augmentation_data_len;

      while (p < end && q < qend)
	{
	  if (*p == 'L')
	    q++;
	  else if (*p == 'P')
	    q += 1 + size_of_encoded_value (*q);
	  else if (*p == 'R')
	    fc->fde_encoding = *q++;
	  else if (*p == 'S')
	    ;
	  else if (*p == 'B')
	    ;
	  else
	    break;
	  p++;
	}
      /* Note - it is OK if this loop terminates with q < qend.
	 Padding may have been inserted to align the end of the CIE.  */
    }

  *p_cie = fc;
  if (p_version)
    *p_version = version;
  if (p_aug_len)
    {
      *p_aug_len = augmentation_data_len;
      *p_aug = augmentation_data;
    }
  return start;

 fail:
  free (fc->col_offset);
  free (fc->col_type);
  free (fc);
  return end;
}