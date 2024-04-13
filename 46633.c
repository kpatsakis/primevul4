void md_error(struct mddev *mddev, struct md_rdev *rdev)
{
	if (!rdev || test_bit(Faulty, &rdev->flags))
		return;

	if (!mddev->pers || !mddev->pers->error_handler)
		return;
	mddev->pers->error_handler(mddev,rdev);
	if (mddev->degraded)
		set_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
	sysfs_notify_dirent_safe(rdev->sysfs_state);
	set_bit(MD_RECOVERY_INTR, &mddev->recovery);
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_wakeup_thread(mddev->thread);
	if (mddev->event_work.func)
		queue_work(md_misc_wq, &mddev->event_work);
	md_new_event_inintr(mddev);
}
