void mddev_resume(struct mddev *mddev)
{
	mddev->suspended = 0;
	wake_up(&mddev->sb_wait);
	mddev->pers->quiesce(mddev, 0);

	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_wakeup_thread(mddev->thread);
	md_wakeup_thread(mddev->sync_thread); /* possibly kick off a reshape */
}
