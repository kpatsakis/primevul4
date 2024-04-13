static int __init netlink_proto_init(void)
{
	int i;
	unsigned long limit;
	unsigned int order;
	int err = proto_register(&netlink_proto, 0);

	if (err != 0)
		goto out;

	BUILD_BUG_ON(sizeof(struct netlink_skb_parms) > FIELD_SIZEOF(struct sk_buff, cb));

	nl_table = kcalloc(MAX_LINKS, sizeof(*nl_table), GFP_KERNEL);
	if (!nl_table)
		goto panic;

	if (totalram_pages >= (128 * 1024))
		limit = totalram_pages >> (21 - PAGE_SHIFT);
	else
		limit = totalram_pages >> (23 - PAGE_SHIFT);

	order = get_bitmask_order(limit) - 1 + PAGE_SHIFT;
	limit = (1UL << order) / sizeof(struct hlist_head);
	order = get_bitmask_order(min(limit, (unsigned long)UINT_MAX)) - 1;

	for (i = 0; i < MAX_LINKS; i++) {
		struct nl_portid_hash *hash = &nl_table[i].hash;

		hash->table = nl_portid_hash_zalloc(1 * sizeof(*hash->table));
		if (!hash->table) {
			while (i-- > 0)
				nl_portid_hash_free(nl_table[i].hash.table,
						 1 * sizeof(*hash->table));
			kfree(nl_table);
			goto panic;
		}
		hash->max_shift = order;
		hash->shift = 0;
		hash->mask = 0;
		hash->rehash_time = jiffies;

		nl_table[i].compare = netlink_compare;
	}

	INIT_LIST_HEAD(&netlink_tap_all);

	netlink_add_usersock_entry();

	sock_register(&netlink_family_ops);
	register_pernet_subsys(&netlink_net_ops);
	/* The netlink device handler may be needed early. */
	rtnetlink_init();
out:
	return err;
panic:
	panic("netlink_init: Cannot allocate nl_table\n");
}
