static struct ctl_dir *xlate_dir(struct ctl_table_set *set, struct ctl_dir *dir)
{
	struct ctl_dir *parent;
	const char *procname;
	if (!dir->header.parent)
		return &set->dir;
	parent = xlate_dir(set, dir->header.parent);
	if (IS_ERR(parent))
		return parent;
	procname = dir->header.ctl_table[0].procname;
	return find_subdir(parent, procname, strlen(procname));
}
