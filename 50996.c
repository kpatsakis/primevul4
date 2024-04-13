static void ffs_func_resume(struct usb_function *f)
{
	ENTER();
	ffs_event_add(ffs_func_from_usb(f)->ffs, FUNCTIONFS_RESUME);
}
