static int md_set_readonly(struct mddev *mddev, struct block_device *bdev)
{
	int err = 0;
	int did_freeze = 0;

	if (!test_bit(MD_RECOVERY_FROZEN, &mddev->recovery)) {
		did_freeze = 1;
		set_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		md_wakeup_thread(mddev->thread);
	}
	if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery))
		set_bit(MD_RECOVERY_INTR, &mddev->recovery);
	if (mddev->sync_thread)
		/* Thread might be blocked waiting for metadata update
		 * which will now never happen */
		wake_up_process(mddev->sync_thread->tsk);

	mddev_unlock(mddev);
	wait_event(resync_wait, !test_bit(MD_RECOVERY_RUNNING,
					  &mddev->recovery));
	mddev_lock_nointr(mddev);

	mutex_lock(&mddev->open_mutex);
	if ((mddev->pers && atomic_read(&mddev->openers) > !!bdev) ||
	    mddev->sync_thread ||
	    test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) ||
	    (bdev && !test_bit(MD_STILL_CLOSED, &mddev->flags))) {
		printk("md: %s still in use.\n",mdname(mddev));
		if (did_freeze) {
			clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
			set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
			md_wakeup_thread(mddev->thread);
		}
		err = -EBUSY;
		goto out;
	}
	if (mddev->pers) {
		__md_stop_writes(mddev);

		err  = -ENXIO;
		if (mddev->ro==1)
			goto out;
		mddev->ro = 1;
		set_disk_ro(mddev->gendisk, 1);
		clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
		md_wakeup_thread(mddev->thread);
		sysfs_notify_dirent_safe(mddev->sysfs_state);
		err = 0;
	}
out:
	mutex_unlock(&mddev->open_mutex);
	return err;
}
