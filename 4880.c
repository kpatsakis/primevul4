prealloc_cu_tu_list (unsigned int nshndx)
{
  if (shndx_pool == NULL)
    {
      shndx_pool_size = nshndx;
      shndx_pool_used = 0;
      shndx_pool = (unsigned int *) xcmalloc (shndx_pool_size,
					      sizeof (unsigned int));
    }
  else
    {
      shndx_pool_size = shndx_pool_used + nshndx;
      shndx_pool = (unsigned int *) xcrealloc (shndx_pool, shndx_pool_size,
					       sizeof (unsigned int));
    }
}