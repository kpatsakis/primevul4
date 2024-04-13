static int hot_add_disk(struct mddev *mddev, dev_t dev)
{
	char b[BDEVNAME_SIZE];
	int err;
	struct md_rdev *rdev;

	if (!mddev->pers)
		return -ENODEV;

	if (mddev->major_version != 0) {
		printk(KERN_WARNING "%s: HOT_ADD may only be used with"
			" version-0 superblocks.\n",
			mdname(mddev));
		return -EINVAL;
	}
	if (!mddev->pers->hot_add_disk) {
		printk(KERN_WARNING
			"%s: personality does not support diskops!\n",
			mdname(mddev));
		return -EINVAL;
	}

	rdev = md_import_device(dev, -1, 0);
	if (IS_ERR(rdev)) {
		printk(KERN_WARNING
			"md: error, md_import_device() returned %ld\n",
			PTR_ERR(rdev));
		return -EINVAL;
	}

	if (mddev->persistent)
		rdev->sb_start = calc_dev_sboffset(rdev);
	else
		rdev->sb_start = i_size_read(rdev->bdev->bd_inode) / 512;

	rdev->sectors = rdev->sb_start;

	if (test_bit(Faulty, &rdev->flags)) {
		printk(KERN_WARNING
			"md: can not hot-add faulty %s disk to %s!\n",
			bdevname(rdev->bdev,b), mdname(mddev));
		err = -EINVAL;
		goto abort_export;
	}

	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_start(mddev);
	clear_bit(In_sync, &rdev->flags);
	rdev->desc_nr = -1;
	rdev->saved_raid_disk = -1;
	err = bind_rdev_to_array(rdev, mddev);
	if (err)
		goto abort_clustered;

	/*
	 * The rest should better be atomic, we can have disk failures
	 * noticed in interrupt contexts ...
	 */

	rdev->raid_disk = -1;

	md_update_sb(mddev, 1);

	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_finish(mddev);
	/*
	 * Kick recovery, maybe this spare has to be added to the
	 * array immediately.
	 */
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_wakeup_thread(mddev->thread);
	md_new_event(mddev);
	return 0;

abort_clustered:
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_cancel(mddev);
abort_export:
	export_rdev(rdev);
	return err;
}
