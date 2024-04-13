int md_allow_write(struct mddev *mddev)
{
	if (!mddev->pers)
		return 0;
	if (mddev->ro)
		return 0;
	if (!mddev->pers->sync_request)
		return 0;

	spin_lock(&mddev->lock);
	if (mddev->in_sync) {
		mddev->in_sync = 0;
		set_bit(MD_CHANGE_CLEAN, &mddev->flags);
		set_bit(MD_CHANGE_PENDING, &mddev->flags);
		if (mddev->safemode_delay &&
		    mddev->safemode == 0)
			mddev->safemode = 1;
		spin_unlock(&mddev->lock);
		if (mddev_is_clustered(mddev))
			md_cluster_ops->metadata_update_start(mddev);
		md_update_sb(mddev, 0);
		if (mddev_is_clustered(mddev))
			md_cluster_ops->metadata_update_finish(mddev);
		sysfs_notify_dirent_safe(mddev->sysfs_state);
	} else
		spin_unlock(&mddev->lock);

	if (test_bit(MD_CHANGE_PENDING, &mddev->flags))
		return -EAGAIN;
	else
		return 0;
}
