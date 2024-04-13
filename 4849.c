add_shndx_to_cu_tu_entry (unsigned int shndx)
{
  if (shndx_pool_used >= shndx_pool_size)
    {
      error (_("Internal error: out of space in the shndx pool.\n"));
      return;
    }
  shndx_pool [shndx_pool_used++] = shndx;
}