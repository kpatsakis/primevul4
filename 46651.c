static int md_notify_reboot(struct notifier_block *this,
			    unsigned long code, void *x)
{
	struct list_head *tmp;
	struct mddev *mddev;
	int need_delay = 0;

	for_each_mddev(mddev, tmp) {
		if (mddev_trylock(mddev)) {
			if (mddev->pers)
				__md_stop_writes(mddev);
			if (mddev->persistent)
				mddev->safemode = 2;
			mddev_unlock(mddev);
		}
		need_delay = 1;
	}
	/*
	 * certain more exotic SCSI devices are known to be
	 * volatile wrt too early system reboots. While the
	 * right place to handle this issue is the given
	 * driver, we do want to have a safe RAID driver ...
	 */
	if (need_delay)
		mdelay(1000*1);

	return NOTIFY_DONE;
}
