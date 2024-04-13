static struct async *async_getpending(struct usb_dev_state *ps,
					     void __user *userurb)
{
	struct async *as;

	list_for_each_entry(as, &ps->async_pending, asynclist)
		if (as->userurb == userurb) {
			list_del_init(&as->asynclist);
			return as;
		}

	return NULL;
}
