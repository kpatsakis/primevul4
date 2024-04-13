action_store(struct mddev *mddev, const char *page, size_t len)
{
	if (!mddev->pers || !mddev->pers->sync_request)
		return -EINVAL;


	if (cmd_match(page, "idle") || cmd_match(page, "frozen")) {
		if (cmd_match(page, "frozen"))
			set_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		else
			clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) &&
		    mddev_lock(mddev) == 0) {
			flush_workqueue(md_misc_wq);
			if (mddev->sync_thread) {
				set_bit(MD_RECOVERY_INTR, &mddev->recovery);
				md_reap_sync_thread(mddev);
			}
			mddev_unlock(mddev);
		}
	} else if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) ||
		   test_bit(MD_RECOVERY_NEEDED, &mddev->recovery))
		return -EBUSY;
	else if (cmd_match(page, "resync"))
		clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
	else if (cmd_match(page, "recover")) {
		clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		set_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
	} else if (cmd_match(page, "reshape")) {
		int err;
		if (mddev->pers->start_reshape == NULL)
			return -EINVAL;
		err = mddev_lock(mddev);
		if (!err) {
			clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
			err = mddev->pers->start_reshape(mddev);
			mddev_unlock(mddev);
		}
		if (err)
			return err;
		sysfs_notify(&mddev->kobj, NULL, "degraded");
	} else {
		if (cmd_match(page, "check"))
			set_bit(MD_RECOVERY_CHECK, &mddev->recovery);
		else if (!cmd_match(page, "repair"))
			return -EINVAL;
		clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
		set_bit(MD_RECOVERY_REQUESTED, &mddev->recovery);
		set_bit(MD_RECOVERY_SYNC, &mddev->recovery);
	}
	if (mddev->ro == 2) {
		/* A write to sync_action is enough to justify
		 * canceling read-auto mode
		 */
		mddev->ro = 0;
		md_wakeup_thread(mddev->sync_thread);
	}
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_wakeup_thread(mddev->thread);
	sysfs_notify_dirent_safe(mddev->sysfs_action);
	return len;
}
