static void md_start_sync(struct work_struct *ws)
{
	struct mddev *mddev = container_of(ws, struct mddev, del_work);

	mddev->sync_thread = md_register_thread(md_do_sync,
						mddev,
						"resync");
	if (!mddev->sync_thread) {
		printk(KERN_ERR "%s: could not start resync"
		       " thread...\n",
		       mdname(mddev));
		/* leave the spares where they are, it shouldn't hurt */
		clear_bit(MD_RECOVERY_SYNC, &mddev->recovery);
		clear_bit(MD_RECOVERY_RESHAPE, &mddev->recovery);
		clear_bit(MD_RECOVERY_REQUESTED, &mddev->recovery);
		clear_bit(MD_RECOVERY_CHECK, &mddev->recovery);
		clear_bit(MD_RECOVERY_RUNNING, &mddev->recovery);
		wake_up(&resync_wait);
		if (test_and_clear_bit(MD_RECOVERY_RECOVER,
				       &mddev->recovery))
			if (mddev->sysfs_action)
				sysfs_notify_dirent_safe(mddev->sysfs_action);
	} else
		md_wakeup_thread(mddev->sync_thread);
	sysfs_notify_dirent_safe(mddev->sysfs_action);
	md_new_event(mddev);
}
