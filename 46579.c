array_size_store(struct mddev *mddev, const char *buf, size_t len)
{
	sector_t sectors;
	int err;

	err = mddev_lock(mddev);
	if (err)
		return err;

	if (strncmp(buf, "default", 7) == 0) {
		if (mddev->pers)
			sectors = mddev->pers->size(mddev, 0, 0);
		else
			sectors = mddev->array_sectors;

		mddev->external_size = 0;
	} else {
		if (strict_blocks_to_sectors(buf, &sectors) < 0)
			err = -EINVAL;
		else if (mddev->pers && mddev->pers->size(mddev, 0, 0) < sectors)
			err = -E2BIG;
		else
			mddev->external_size = 1;
	}

	if (!err) {
		mddev->array_sectors = sectors;
		if (mddev->pers) {
			set_capacity(mddev->gendisk, mddev->array_sectors);
			revalidate_disk(mddev->gendisk);
		}
	}
	mddev_unlock(mddev);
	return err ?: len;
}
