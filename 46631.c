void md_done_sync(struct mddev *mddev, int blocks, int ok)
{
	/* another "blocks" (512byte) blocks have been synced */
	atomic_sub(blocks, &mddev->recovery_active);
	wake_up(&mddev->recovery_wait);
	if (!ok) {
		set_bit(MD_RECOVERY_INTR, &mddev->recovery);
		set_bit(MD_RECOVERY_ERROR, &mddev->recovery);
		md_wakeup_thread(mddev->thread);
	}
}
