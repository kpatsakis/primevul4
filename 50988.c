static void ffs_free(struct usb_function *f)
{
	kfree(ffs_func_from_usb(f));
}
