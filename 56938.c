grub_disk_dev_register (grub_disk_dev_t dev)
{
  dev->next = grub_disk_dev_list;
  grub_disk_dev_list = dev;
}
