void md_reap_sync_thread(struct mddev *mddev)
{
	struct md_rdev *rdev;

	/* resync has finished, collect result */
	md_unregister_thread(&mddev->sync_thread);
	if (!test_bit(MD_RECOVERY_INTR, &mddev->recovery) &&
	    !test_bit(MD_RECOVERY_REQUESTED, &mddev->recovery)) {
		/* success...*/
		/* activate any spares */
		if (mddev->pers->spare_active(mddev)) {
			sysfs_notify(&mddev->kobj, NULL,
				     "degraded");
			set_bit(MD_CHANGE_DEVS, &mddev->flags);
		}
	}
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_start(mddev);
	if (test_bit(MD_RECOVERY_RESHAPE, &mddev->recovery) &&
	    mddev->pers->finish_reshape)
		mddev->pers->finish_reshape(mddev);

	/* If array is no-longer degraded, then any saved_raid_disk
	 * information must be scrapped.
	 */
	if (!mddev->degraded)
		rdev_for_each(rdev, mddev)
			rdev->saved_raid_disk = -1;

	md_update_sb(mddev, 1);
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_finish(mddev);
	clear_bit(MD_RECOVERY_RUNNING, &mddev->recovery);
	clear_bit(MD_RECOVERY_DONE, &mddev->recovery);
	clear_bit(MD_RECOVERY_SYNC, &mddev->recovery);
	clear_bit(MD_RECOVERY_RESHAPE, &mddev->recovery);
	clear_bit(MD_RECOVERY_REQUESTED, &mddev->recovery);
	clear_bit(MD_RECOVERY_CHECK, &mddev->recovery);
	wake_up(&resync_wait);
	/* flag recovery needed just to double check */
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	sysfs_notify_dirent_safe(mddev->sysfs_action);
	md_new_event(mddev);
	if (mddev->event_work.func)
		queue_work(md_misc_wq, &mddev->event_work);
}
