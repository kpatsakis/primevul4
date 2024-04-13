static int __init debugfs_init(void)
{
	int retval;

	retval = sysfs_create_mount_point(kernel_kobj, "debug");
	if (retval)
		return retval;

	retval = register_filesystem(&debug_fs_type);
	if (retval)
		sysfs_remove_mount_point(kernel_kobj, "debug");
	else
		debugfs_registered = true;

	return retval;
}
