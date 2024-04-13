static long cuse_file_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	struct fuse_file *ff = file->private_data;
	struct cuse_conn *cc = fc_to_cc(ff->fc);
	unsigned int flags = 0;

	if (cc->unrestricted_ioctl)
		flags |= FUSE_IOCTL_UNRESTRICTED;

	return fuse_do_ioctl(file, cmd, arg, flags);
}
