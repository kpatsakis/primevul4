static int dname_to_vma_addr(struct dentry *dentry,
			     unsigned long *start, unsigned long *end)
{
	if (sscanf(dentry->d_name.name, "%lx-%lx", start, end) != 2)
		return -EINVAL;

	return 0;
}
