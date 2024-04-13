static void rt_add_uncached_list(struct rtable *rt)
{
	struct uncached_list *ul = raw_cpu_ptr(&rt_uncached_list);

	rt->rt_uncached_list = ul;

	spin_lock_bh(&ul->lock);
	list_add_tail(&rt->rt_uncached, &ul->head);
	spin_unlock_bh(&ul->lock);
}
