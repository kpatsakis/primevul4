static void __md_stop_writes(struct mddev *mddev)
{
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_start(mddev);
	set_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
	flush_workqueue(md_misc_wq);
	if (mddev->sync_thread) {
		set_bit(MD_RECOVERY_INTR, &mddev->recovery);
		md_reap_sync_thread(mddev);
	}

	del_timer_sync(&mddev->safemode_timer);

	bitmap_flush(mddev);
	md_super_wait(mddev);

	if (mddev->ro == 0 &&
	    (!mddev->in_sync || (mddev->flags & MD_UPDATE_SB_FLAGS))) {
		/* mark array as shutdown cleanly */
		mddev->in_sync = 1;
		md_update_sb(mddev, 1);
	}
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_finish(mddev);
}
