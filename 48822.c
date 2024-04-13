static int exofs_flush(struct file *file, fl_owner_t id)
{
	int ret = vfs_fsync(file, 0);
	/* TODO: Flush the OSD target */
	return ret;
}
