static int bsg_open(struct inode *inode, struct file *file)
{
	struct bsg_device *bd;

	bd = bsg_get_device(inode, file);

	if (IS_ERR(bd))
		return PTR_ERR(bd);

	file->private_data = bd;
	return 0;
}
