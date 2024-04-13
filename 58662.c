void shrink_dcache_parent(struct dentry *parent)
{
	for (;;) {
		struct select_data data;

		INIT_LIST_HEAD(&data.dispose);
		data.start = parent;
		data.found = 0;

		d_walk(parent, &data, select_collect, NULL);
		if (!data.found)
			break;

		shrink_dentry_list(&data.dispose);
		cond_resched();
	}
}
