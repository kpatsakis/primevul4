static struct oz_urb_link *oz_alloc_urb_link(void)
{
	return kmem_cache_alloc(oz_urb_link_cache, GFP_ATOMIC);
}
