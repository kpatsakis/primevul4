void __init tcp_init(void)
{
	struct sk_buff *skb = NULL;
	unsigned long nr_pages, limit;
	int order, i, max_share;
	unsigned long jiffy = jiffies;

	BUILD_BUG_ON(sizeof(struct tcp_skb_cb) > sizeof(skb->cb));

	percpu_counter_init(&tcp_sockets_allocated, 0);
	percpu_counter_init(&tcp_orphan_count, 0);
	tcp_hashinfo.bind_bucket_cachep =
		kmem_cache_create("tcp_bind_bucket",
				  sizeof(struct inet_bind_bucket), 0,
				  SLAB_HWCACHE_ALIGN|SLAB_PANIC, NULL);

	/* Size and allocate the main established and bind bucket
	 * hash tables.
	 *
	 * The methodology is similar to that of the buffer cache.
	 */
	tcp_hashinfo.ehash =
		alloc_large_system_hash("TCP established",
					sizeof(struct inet_ehash_bucket),
					thash_entries,
					(totalram_pages >= 128 * 1024) ?
					13 : 15,
					0,
					NULL,
					&tcp_hashinfo.ehash_mask,
					thash_entries ? 0 : 512 * 1024);
	for (i = 0; i <= tcp_hashinfo.ehash_mask; i++) {
		INIT_HLIST_NULLS_HEAD(&tcp_hashinfo.ehash[i].chain, i);
		INIT_HLIST_NULLS_HEAD(&tcp_hashinfo.ehash[i].twchain, i);
	}
	if (inet_ehash_locks_alloc(&tcp_hashinfo))
		panic("TCP: failed to alloc ehash_locks");
	tcp_hashinfo.bhash =
		alloc_large_system_hash("TCP bind",
					sizeof(struct inet_bind_hashbucket),
					tcp_hashinfo.ehash_mask + 1,
					(totalram_pages >= 128 * 1024) ?
					13 : 15,
					0,
					&tcp_hashinfo.bhash_size,
					NULL,
					64 * 1024);
	tcp_hashinfo.bhash_size = 1 << tcp_hashinfo.bhash_size;
	for (i = 0; i < tcp_hashinfo.bhash_size; i++) {
		spin_lock_init(&tcp_hashinfo.bhash[i].lock);
		INIT_HLIST_HEAD(&tcp_hashinfo.bhash[i].chain);
	}

	/* Try to be a bit smarter and adjust defaults depending
	 * on available memory.
	 */
	for (order = 0; ((1 << order) << PAGE_SHIFT) <
			(tcp_hashinfo.bhash_size * sizeof(struct inet_bind_hashbucket));
			order++)
		;
	if (order >= 4) {
		tcp_death_row.sysctl_max_tw_buckets = 180000;
		sysctl_tcp_max_orphans = 4096 << (order - 4);
		sysctl_max_syn_backlog = 1024;
	} else if (order < 3) {
		tcp_death_row.sysctl_max_tw_buckets >>= (3 - order);
		sysctl_tcp_max_orphans >>= (3 - order);
		sysctl_max_syn_backlog = 128;
	}

	/* Set the pressure threshold to be a fraction of global memory that
	 * is up to 1/2 at 256 MB, decreasing toward zero with the amount of
	 * memory, with a floor of 128 pages.
	 */
	nr_pages = totalram_pages - totalhigh_pages;
	limit = min(nr_pages, 1UL<<(28-PAGE_SHIFT)) >> (20-PAGE_SHIFT);
	limit = (limit * (nr_pages >> (20-PAGE_SHIFT))) >> (PAGE_SHIFT-11);
	limit = max(limit, 128UL);
	sysctl_tcp_mem[0] = limit / 4 * 3;
	sysctl_tcp_mem[1] = limit;
	sysctl_tcp_mem[2] = sysctl_tcp_mem[0] * 2;

	/* Set per-socket limits to no more than 1/128 the pressure threshold */
	limit = ((unsigned long)sysctl_tcp_mem[1]) << (PAGE_SHIFT - 7);
	max_share = min(4UL*1024*1024, limit);

	sysctl_tcp_wmem[0] = SK_MEM_QUANTUM;
	sysctl_tcp_wmem[1] = 16*1024;
	sysctl_tcp_wmem[2] = max(64*1024, max_share);

	sysctl_tcp_rmem[0] = SK_MEM_QUANTUM;
	sysctl_tcp_rmem[1] = 87380;
	sysctl_tcp_rmem[2] = max(87380, max_share);

	printk(KERN_INFO "TCP: Hash tables configured "
	       "(established %u bind %u)\n",
	       tcp_hashinfo.ehash_mask + 1, tcp_hashinfo.bhash_size);

	tcp_register_congestion_control(&tcp_reno);

	memset(&tcp_secret_one.secrets[0], 0, sizeof(tcp_secret_one.secrets));
	memset(&tcp_secret_two.secrets[0], 0, sizeof(tcp_secret_two.secrets));
	tcp_secret_one.expires = jiffy; /* past due */
	tcp_secret_two.expires = jiffy; /* past due */
	tcp_secret_generating = &tcp_secret_one;
	tcp_secret_primary = &tcp_secret_one;
	tcp_secret_retiring = &tcp_secret_two;
	tcp_secret_secondary = &tcp_secret_two;
}
