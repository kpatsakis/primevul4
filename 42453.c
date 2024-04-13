bm_entry_read(struct file * file, char __user * buf, size_t nbytes, loff_t *ppos)
{
	Node *e = file->f_path.dentry->d_inode->i_private;
	ssize_t res;
	char *page;

	if (!(page = (char*) __get_free_page(GFP_KERNEL)))
		return -ENOMEM;

	entry_status(e, page);

	res = simple_read_from_buffer(buf, nbytes, ppos, page, strlen(page));

	free_page((unsigned long) page);
	return res;
}
