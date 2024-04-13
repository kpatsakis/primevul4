static void md_safemode_timeout(unsigned long data)
{
	struct mddev *mddev = (struct mddev *) data;

	if (!atomic_read(&mddev->writes_pending)) {
		mddev->safemode = 1;
		if (mddev->external)
			sysfs_notify_dirent_safe(mddev->sysfs_state);
	}
	md_wakeup_thread(mddev->thread);
}
