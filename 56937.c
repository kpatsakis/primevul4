grub_disk_dev_iterate (int (*hook) (const char *name, void *closure),
		       void *closure)
{
  grub_disk_dev_t p;

  for (p = grub_disk_dev_list; p; p = p->next)
    if (p->iterate && (p->iterate) (hook, closure))
      return 1;

  return 0;
}
