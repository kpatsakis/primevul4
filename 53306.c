void console_sysfs_notify(void)
{
	if (consdev)
		sysfs_notify(&consdev->kobj, NULL, "active");
}
