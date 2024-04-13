grub_disk_close (grub_disk_t disk)
{
  grub_partition_t part;
  grub_dprintf ("disk", "Closing `%s'.\n", disk->name);

  if (disk->dev && disk->dev->close)
    (disk->dev->close) (disk);

  /* Reset the timer.  */
  grub_last_time = grub_get_time_ms ();

  while (disk->partition)
    {
      part = disk->partition->parent;
      grub_free (disk->partition);
      disk->partition = part;
    }
  grub_free ((void *) disk->name);
  grub_free (disk);
}
