static long vhost_scsi_compat_ioctl(struct file *f, unsigned int ioctl,
				unsigned long arg)
{
	return vhost_scsi_ioctl(f, ioctl, (unsigned long)compat_ptr(arg));
}
