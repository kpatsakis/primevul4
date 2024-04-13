static struct async *alloc_async(unsigned int numisoframes)
{
	struct async *as;

	as = kzalloc(sizeof(struct async), GFP_KERNEL);
	if (!as)
		return NULL;
	as->urb = usb_alloc_urb(numisoframes, GFP_KERNEL);
	if (!as->urb) {
		kfree(as);
		return NULL;
	}
	return as;
}
