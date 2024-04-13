void btrfs_dev_stat_inc_and_print(struct btrfs_device *dev, int index)
{
	btrfs_dev_stat_inc(dev, index);
	btrfs_dev_stat_print_on_error(dev);
}