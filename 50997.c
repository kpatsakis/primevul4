static int ffs_func_revmap_ep(struct ffs_function *func, u8 num)
{
	num = func->eps_revmap[num & USB_ENDPOINT_NUMBER_MASK];
	return num ? num : -EDOM;
}
