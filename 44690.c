static int fd_ioctl(struct block_device *bdev, fmode_t mode,
			     unsigned int cmd, unsigned long param)
{
	int ret;

	mutex_lock(&floppy_mutex);
	ret = fd_locked_ioctl(bdev, mode, cmd, param);
	mutex_unlock(&floppy_mutex);

	return ret;
}
