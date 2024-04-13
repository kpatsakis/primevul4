static void oz_free_urb_link(struct oz_urb_link *urbl)
{
	if (!urbl)
		return;

	kmem_cache_free(oz_urb_link_cache, urbl);
}
