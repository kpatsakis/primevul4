static int comedi_fasync(int fd, struct file *file, int on)
{
	const unsigned minor = iminor(file->f_dentry->d_inode);
	struct comedi_device_file_info *dev_file_info =
	    comedi_get_device_file_info(minor);

	struct comedi_device *dev = dev_file_info->device;

	return fasync_helper(fd, file, on, &dev->async_queue);
}
