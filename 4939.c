static int devid_cmp(void *priv, const struct list_head *a,
		     const struct list_head *b)
{
	const struct btrfs_device *dev1, *dev2;

	dev1 = list_entry(a, struct btrfs_device, dev_list);
	dev2 = list_entry(b, struct btrfs_device, dev_list);

	if (dev1->devid < dev2->devid)
		return -1;
	else if (dev1->devid > dev2->devid)
		return 1;
	return 0;
}