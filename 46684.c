void md_write_start(struct mddev *mddev, struct bio *bi)
{
	int did_change = 0;
	if (bio_data_dir(bi) != WRITE)
		return;

	BUG_ON(mddev->ro == 1);
	if (mddev->ro == 2) {
		/* need to switch to read/write */
		mddev->ro = 0;
		set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
		md_wakeup_thread(mddev->thread);
		md_wakeup_thread(mddev->sync_thread);
		did_change = 1;
	}
	atomic_inc(&mddev->writes_pending);
	if (mddev->safemode == 1)
		mddev->safemode = 0;
	if (mddev->in_sync) {
		spin_lock(&mddev->lock);
		if (mddev->in_sync) {
			mddev->in_sync = 0;
			set_bit(MD_CHANGE_CLEAN, &mddev->flags);
			set_bit(MD_CHANGE_PENDING, &mddev->flags);
			md_wakeup_thread(mddev->thread);
			did_change = 1;
		}
		spin_unlock(&mddev->lock);
	}
	if (did_change)
		sysfs_notify_dirent_safe(mddev->sysfs_state);
	wait_event(mddev->sb_wait,
		   !test_bit(MD_CHANGE_PENDING, &mddev->flags));
}
