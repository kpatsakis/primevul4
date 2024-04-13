static struct ffs_function *ffs_func_from_usb(struct usb_function *f)
{
	return container_of(f, struct ffs_function, function);
}
