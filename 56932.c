grub_disk_adjust_range (grub_disk_t disk, grub_disk_addr_t *sector,
			grub_off_t *offset, grub_size_t size)
{
  *sector += *offset >> GRUB_DISK_SECTOR_BITS;
  *offset &= GRUB_DISK_SECTOR_SIZE - 1;
/*
  grub_partition_t part;
  for (part = disk->partition; part; part = part->parent)
    {
      grub_disk_addr_t start;
      grub_uint64_t len;

      start = part->start;
      len = part->len;

      if (*sector >= len
	  || len - *sector < ((*offset + size + GRUB_DISK_SECTOR_SIZE - 1)
			      >> GRUB_DISK_SECTOR_BITS))
	return grub_error (GRUB_ERR_OUT_OF_RANGE, "out of partition");

      *sector += start;
    }

  if (disk->total_sectors <= *sector
      || ((*offset + size + GRUB_DISK_SECTOR_SIZE - 1)
	  >> GRUB_DISK_SECTOR_BITS) > disk->total_sectors - *sector)
    return grub_error (GRUB_ERR_OUT_OF_RANGE, "out of disk");
*/
  return GRUB_ERR_NONE;

}
