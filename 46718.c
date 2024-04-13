static int restart_array(struct mddev *mddev)
{
	struct gendisk *disk = mddev->gendisk;

	/* Complain if it has no devices */
	if (list_empty(&mddev->disks))
		return -ENXIO;
	if (!mddev->pers)
		return -EINVAL;
	if (!mddev->ro)
		return -EBUSY;
	mddev->safemode = 0;
	mddev->ro = 0;
	set_disk_ro(disk, 0);
	printk(KERN_INFO "md: %s switched to read-write mode.\n",
		mdname(mddev));
	/* Kick recovery or resync if necessary */
	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);
	md_wakeup_thread(mddev->thread);
	md_wakeup_thread(mddev->sync_thread);
	sysfs_notify_dirent_safe(mddev->sysfs_state);
	return 0;
}
