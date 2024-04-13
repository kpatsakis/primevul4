void md_check_recovery(struct mddev *mddev)
{
	if (mddev->suspended)
		return;

	if (mddev->bitmap)
		bitmap_daemon_work(mddev);

	if (signal_pending(current)) {
		if (mddev->pers->sync_request && !mddev->external) {
			printk(KERN_INFO "md: %s in immediate safe mode\n",
			       mdname(mddev));
			mddev->safemode = 2;
		}
		flush_signals(current);
	}

	if (mddev->ro && !test_bit(MD_RECOVERY_NEEDED, &mddev->recovery))
		return;
	if ( ! (
		(mddev->flags & MD_UPDATE_SB_FLAGS & ~ (1<<MD_CHANGE_PENDING)) ||
		test_bit(MD_RECOVERY_NEEDED, &mddev->recovery) ||
		test_bit(MD_RECOVERY_DONE, &mddev->recovery) ||
		(mddev->external == 0 && mddev->safemode == 1) ||
		(mddev->safemode == 2 && ! atomic_read(&mddev->writes_pending)
		 && !mddev->in_sync && mddev->recovery_cp == MaxSector)
		))
		return;

	if (mddev_trylock(mddev)) {
		int spares = 0;

		if (mddev->ro) {
			struct md_rdev *rdev;
			if (!mddev->external && mddev->in_sync)
				/* 'Blocked' flag not needed as failed devices
				 * will be recorded if array switched to read/write.
				 * Leaving it set will prevent the device
				 * from being removed.
				 */
				rdev_for_each(rdev, mddev)
					clear_bit(Blocked, &rdev->flags);
			/* On a read-only array we can:
			 * - remove failed devices
			 * - add already-in_sync devices if the array itself
			 *   is in-sync.
			 * As we only add devices that are already in-sync,
			 * we can activate the spares immediately.
			 */
			remove_and_add_spares(mddev, NULL);
			/* There is no thread, but we need to call
			 * ->spare_active and clear saved_raid_disk
			 */
			set_bit(MD_RECOVERY_INTR, &mddev->recovery);
			md_reap_sync_thread(mddev);
			clear_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
			goto unlock;
		}

		if (!mddev->external) {
			int did_change = 0;
			spin_lock(&mddev->lock);
			if (mddev->safemode &&
			    !atomic_read(&mddev->writes_pending) &&
			    !mddev->in_sync &&
			    mddev->recovery_cp == MaxSector) {
				mddev->in_sync = 1;
				did_change = 1;
				set_bit(MD_CHANGE_CLEAN, &mddev->flags);
			}
			if (mddev->safemode == 1)
				mddev->safemode = 0;
			spin_unlock(&mddev->lock);
			if (did_change)
				sysfs_notify_dirent_safe(mddev->sysfs_state);
		}

		if (mddev->flags & MD_UPDATE_SB_FLAGS) {
			if (mddev_is_clustered(mddev))
				md_cluster_ops->metadata_update_start(mddev);
			md_update_sb(mddev, 0);
			if (mddev_is_clustered(mddev))
				md_cluster_ops->metadata_update_finish(mddev);
		}

		if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) &&
		    !test_bit(MD_RECOVERY_DONE, &mddev->recovery)) {
			/* resync/recovery still happening */
			clear_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
			goto unlock;
		}
		if (mddev->sync_thread) {
			md_reap_sync_thread(mddev);
			goto unlock;
		}
		/* Set RUNNING before clearing NEEDED to avoid
		 * any transients in the value of "sync_action".
		 */
		mddev->curr_resync_completed = 0;
		spin_lock(&mddev->lock);
		set_bit(MD_RECOVERY_RUNNING, &mddev->recovery);
		spin_unlock(&mddev->lock);
		/* Clear some bits that don't mean anything, but
		 * might be left set
		 */
		clear_bit(MD_RECOVERY_INTR, &mddev->recovery);
		clear_bit(MD_RECOVERY_DONE, &mddev->recovery);

		if (!test_and_clear_bit(MD_RECOVERY_NEEDED, &mddev->recovery) ||
		    test_bit(MD_RECOVERY_FROZEN, &mddev->recovery))
			goto not_running;
		/* no recovery is running.
		 * remove any failed drives, then
		 * add spares if possible.
		 * Spares are also removed and re-added, to allow
		 * the personality to fail the re-add.
		 */

		if (mddev->reshape_position != MaxSector) {
			if (mddev->pers->check_reshape == NULL ||
			    mddev->pers->check_reshape(mddev) != 0)
				/* Cannot proceed */
				goto not_running;
			set_bit(MD_RECOVERY_RESHAPE, &mddev->recovery);
			clear_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
		} else if ((spares = remove_and_add_spares(mddev, NULL))) {
			clear_bit(MD_RECOVERY_SYNC, &mddev->recovery);
			clear_bit(MD_RECOVERY_CHECK, &mddev->recovery);
			clear_bit(MD_RECOVERY_REQUESTED, &mddev->recovery);
			set_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
		} else if (mddev->recovery_cp < MaxSector) {
			set_bit(MD_RECOVERY_SYNC, &mddev->recovery);
			clear_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
		} else if (!test_bit(MD_RECOVERY_SYNC, &mddev->recovery))
			/* nothing to be done ... */
			goto not_running;

		if (mddev->pers->sync_request) {
			if (spares) {
				/* We are adding a device or devices to an array
				 * which has the bitmap stored on all devices.
				 * So make sure all bitmap pages get written
				 */
				bitmap_write_all(mddev->bitmap);
			}
			INIT_WORK(&mddev->del_work, md_start_sync);
			queue_work(md_misc_wq, &mddev->del_work);
			goto unlock;
		}
	not_running:
		if (!mddev->sync_thread) {
			clear_bit(MD_RECOVERY_RUNNING, &mddev->recovery);
			wake_up(&resync_wait);
			if (test_and_clear_bit(MD_RECOVERY_RECOVER,
					       &mddev->recovery))
				if (mddev->sysfs_action)
					sysfs_notify_dirent_safe(mddev->sysfs_action);
		}
	unlock:
		wake_up(&mddev->sb_wait);
		mddev_unlock(mddev);
	}
}
