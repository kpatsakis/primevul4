static int add_bound_rdev(struct md_rdev *rdev)
{
	struct mddev *mddev = rdev->mddev;
	int err = 0;

	if (!mddev->pers->hot_remove_disk) {
		/* If there is hot_add_disk but no hot_remove_disk
		 * then added disks for geometry changes,
		 * and should be added immediately.
		 */
		super_types[mddev->major_version].
			validate_super(mddev, rdev);
		err = mddev->pers->hot_add_disk(mddev, rdev);
		if (err) {
			unbind_rdev_from_array(rdev);
			export_rdev(rdev);
			return err;
		}
	}
	sysfs_notify_dirent_safe(rdev->sysfs_state);

	set_bit(MD_CHANGE_DEVS, &mddev->flags);
	if (mddev->degraded)
		set_bit(MD_RECOVERY_RECOVER, &mddev->recovery);
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_new_event(mddev);
	md_wakeup_thread(mddev->thread);
	return 0;
}
