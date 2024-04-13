static void set_empty_dir(struct ctl_dir *dir)
{
	dir->header.ctl_table[0].child = sysctl_mount_point;
}
