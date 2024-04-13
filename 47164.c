static struct oz_endpoint *oz_ep_alloc(int buffer_size, gfp_t mem_flags)
{
	struct oz_endpoint *ep;

	ep = kzalloc(sizeof(struct oz_endpoint)+buffer_size, mem_flags);
	if (!ep)
		return NULL;

	INIT_LIST_HEAD(&ep->urb_list);
	INIT_LIST_HEAD(&ep->link);
	ep->credit = -1;
	if (buffer_size) {
		ep->buffer_size = buffer_size;
		ep->buffer = (u8 *)(ep+1);
	}

	return ep;
}
