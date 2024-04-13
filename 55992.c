static void ipv4_dst_destroy(struct dst_entry *dst)
{
	struct dst_metrics *p = (struct dst_metrics *)DST_METRICS_PTR(dst);
	struct rtable *rt = (struct rtable *) dst;

	if (p != &dst_default_metrics && atomic_dec_and_test(&p->refcnt))
		kfree(p);

	if (!list_empty(&rt->rt_uncached)) {
		struct uncached_list *ul = rt->rt_uncached_list;

		spin_lock_bh(&ul->lock);
		list_del(&rt->rt_uncached);
		spin_unlock_bh(&ul->lock);
	}
}
