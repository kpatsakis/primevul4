void md_set_array_sectors(struct mddev *mddev, sector_t array_sectors)
{
	WARN(!mddev_is_locked(mddev), "%s: unlocked mddev!\n", __func__);

	if (mddev->external_size)
		return;

	mddev->array_sectors = array_sectors;
}
