static void sysctl_print_dir(struct ctl_dir *dir)
{
	if (dir->header.parent)
		sysctl_print_dir(dir->header.parent);
	pr_cont("%s/", dir->header.ctl_table[0].procname);
}
