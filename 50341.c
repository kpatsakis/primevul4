static void clear_empty_dir(struct ctl_dir *dir)

{
	dir->header.ctl_table[0].child = NULL;
}
