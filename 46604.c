static int hot_remove_disk(struct mddev *mddev, dev_t dev)
{
	char b[BDEVNAME_SIZE];
	struct md_rdev *rdev;

	rdev = find_rdev(mddev, dev);
	if (!rdev)
		return -ENXIO;

	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_start(mddev);

	clear_bit(Blocked, &rdev->flags);
	remove_and_add_spares(mddev, rdev);

	if (rdev->raid_disk >= 0)
		goto busy;

	if (mddev_is_clustered(mddev))
		md_cluster_ops->remove_disk(mddev, rdev);

	md_kick_rdev_from_array(rdev);
	md_update_sb(mddev, 1);
	md_new_event(mddev);

	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_finish(mddev);

	return 0;
busy:
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_cancel(mddev);
	printk(KERN_WARNING "md: cannot remove active disk %s from %s ...\n",
		bdevname(rdev->bdev,b), mdname(mddev));
	return -EBUSY;
}
