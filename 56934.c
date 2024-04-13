grub_disk_cache_invalidate_all (void)
{
  unsigned i;

  for (i = 0; i < GRUB_DISK_CACHE_NUM; i++)
    {
      struct grub_disk_cache *cache = grub_disk_cache_table + i;

      if (cache->data && ! cache->lock)
	{
	  grub_free (cache->data);
	  cache->data = 0;
	}
    }
}
