find_cu_tu_set (void *file, unsigned int shndx)
{
  unsigned int i;

  if (! load_cu_tu_indexes (file))
    return NULL;

  /* Find SHNDX in the shndx pool.  */
  for (i = 0; i < shndx_pool_used; i++)
    if (shndx_pool [i] == shndx)
      break;

  if (i >= shndx_pool_used)
    return NULL;

  /* Now backup to find the first entry in the set.  */
  while (i > 0 && shndx_pool [i - 1] != 0)
    i--;

  return shndx_pool + i;
}