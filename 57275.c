grub_disk_t grubfs_disk (void *data) {
	struct grub_disk *disk = empty (sizeof (struct grub_disk));
	disk->dev = empty (sizeof (struct grub_disk_dev));
	disk->dev->read = read_foo; // grub_disk_dev
	disk->data = data;
	return disk;
}
