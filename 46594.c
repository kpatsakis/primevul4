static int do_md_stop(struct mddev *mddev, int mode,
		      struct block_device *bdev)
{
	struct gendisk *disk = mddev->gendisk;
	struct md_rdev *rdev;
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
	wait_event(resync_wait, (mddev->sync_thread == NULL &&
				 !test_bit(MD_RECOVERY_RUNNING,
					   &mddev->recovery)));
	mddev_lock_nointr(mddev);

	mutex_lock(&mddev->open_mutex);
	if ((mddev->pers && atomic_read(&mddev->openers) > !!bdev) ||
	    mddev->sysfs_active ||
	    mddev->sync_thread ||
	    test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) ||
	    (bdev && !test_bit(MD_STILL_CLOSED, &mddev->flags))) {
		printk("md: %s still in use.\n",mdname(mddev));
		mutex_unlock(&mddev->open_mutex);
		if (did_freeze) {
			clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
			set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
			md_wakeup_thread(mddev->thread);
		}
		return -EBUSY;
	}
	if (mddev->pers) {
		if (mddev->ro)
			set_disk_ro(disk, 0);

		__md_stop_writes(mddev);
		__md_stop(mddev);
		mddev->queue->merge_bvec_fn = NULL;
		mddev->queue->backing_dev_info.congested_fn = NULL;

		/* tell userspace to handle 'inactive' */
		sysfs_notify_dirent_safe(mddev->sysfs_state);

		rdev_for_each(rdev, mddev)
			if (rdev->raid_disk >= 0)
				sysfs_unlink_rdev(mddev, rdev);

		set_capacity(disk, 0);
		mutex_unlock(&mddev->open_mutex);
		mddev->changed = 1;
		revalidate_disk(disk);

		if (mddev->ro)
			mddev->ro = 0;
	} else
		mutex_unlock(&mddev->open_mutex);
	/*
	 * Free resources if final stop
	 */
	if (mode == 0) {
		printk(KERN_INFO "md: %s stopped.\n", mdname(mddev));

		bitmap_destroy(mddev);
		if (mddev->bitmap_info.file) {
			struct file *f = mddev->bitmap_info.file;
			spin_lock(&mddev->lock);
			mddev->bitmap_info.file = NULL;
			spin_unlock(&mddev->lock);
			fput(f);
		}
		mddev->bitmap_info.offset = 0;

		export_array(mddev);

		md_clean(mddev);
		kobject_uevent(&disk_to_dev(mddev->gendisk)->kobj, KOBJ_CHANGE);
		if (mddev->hold_active == UNTIL_STOP)
			mddev->hold_active = 0;
	}
	blk_integrity_unregister(disk);
	md_new_event(mddev);
	sysfs_notify_dirent_safe(mddev->sysfs_state);
	return 0;
}
