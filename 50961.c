static struct usb_function *ffs_alloc(struct usb_function_instance *fi)
{
	struct ffs_function *func;

	ENTER();

	func = kzalloc(sizeof(*func), GFP_KERNEL);
	if (unlikely(!func))
		return ERR_PTR(-ENOMEM);

	func->function.name    = "Function FS Gadget";

	func->function.bind    = ffs_func_bind;
	func->function.unbind  = ffs_func_unbind;
	func->function.set_alt = ffs_func_set_alt;
	func->function.disable = ffs_func_disable;
	func->function.setup   = ffs_func_setup;
	func->function.suspend = ffs_func_suspend;
	func->function.resume  = ffs_func_resume;
	func->function.free_func = ffs_free;

	return &func->function;
}
