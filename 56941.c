grub_disk_read_ex (grub_disk_t disk, grub_disk_addr_t sector,
		   grub_off_t offset, grub_size_t size, void *buf, int flags)
{
  unsigned real_offset;

  if (! flags)
    return grub_disk_read (disk, sector, offset, size, buf);

  if (grub_disk_adjust_range (disk, &sector, &offset, size) != GRUB_ERR_NONE)
    return grub_errno;

  real_offset = offset;
  while (size)
    {
      char tmp_buf[GRUB_DISK_SECTOR_SIZE];
      grub_size_t len;

      if ((real_offset != 0) || (size < GRUB_DISK_SECTOR_SIZE))
	{
	  len = GRUB_DISK_SECTOR_SIZE - real_offset;
	  if (len > size)
	    len = size;

	  if (buf)
	    {
	      if ((disk->dev->read) (disk, sector, 1, tmp_buf) != GRUB_ERR_NONE)
		break;
	      grub_memcpy (buf, tmp_buf + real_offset, len);
	    }

	  if (disk->read_hook)
	    (disk->read_hook) (sector, real_offset, len, disk->closure);

	  sector++;
	  real_offset = 0;
	}
      else
	{
	  grub_size_t n;

	  len = size & ~(GRUB_DISK_SECTOR_SIZE - 1);
	  n = size >> GRUB_DISK_SECTOR_BITS;

	  if ((buf) &&
	      ((disk->dev->read) (disk, sector, n, buf) != GRUB_ERR_NONE))
	    break;

	  if (disk->read_hook)
	    {
	      while (n)
		{
		  (disk->read_hook) (sector++, 0, GRUB_DISK_SECTOR_SIZE,
				     disk->closure);
		  n--;
		}
	    }
	  else
	    sector += n;
	}

      if (buf)
	buf = (char *) buf + len;
      size -= len;
    }

  return grub_errno;
}
