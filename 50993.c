static void ffs_func_disable(struct usb_function *f)
{
	ffs_func_set_alt(f, 0, (unsigned)-1);
}
