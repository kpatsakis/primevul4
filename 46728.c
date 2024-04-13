size_store(struct mddev *mddev, const char *buf, size_t len)
{
	/* If array is inactive, we can reduce the component size, but
	 * not increase it (except from 0).
	 * If array is active, we can try an on-line resize
	 */
	sector_t sectors;
	int err = strict_blocks_to_sectors(buf, &sectors);

	if (err < 0)
		return err;
	err = mddev_lock(mddev);
	if (err)
		return err;
	if (mddev->pers) {
		if (mddev_is_clustered(mddev))
			md_cluster_ops->metadata_update_start(mddev);
		err = update_size(mddev, sectors);
		md_update_sb(mddev, 1);
		if (mddev_is_clustered(mddev))
			md_cluster_ops->metadata_update_finish(mddev);
	} else {
		if (mddev->dev_sectors == 0 ||
		    mddev->dev_sectors > sectors)
			mddev->dev_sectors = sectors;
		else
			err = -ENOSPC;
	}
	mddev_unlock(mddev);
	return err ? err : len;
}
