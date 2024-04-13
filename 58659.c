static enum d_walk_ret select_collect(void *_data, struct dentry *dentry)
{
	struct select_data *data = _data;
	enum d_walk_ret ret = D_WALK_CONTINUE;

	if (data->start == dentry)
		goto out;

	if (dentry->d_flags & DCACHE_SHRINK_LIST) {
		data->found++;
	} else {
		if (dentry->d_flags & DCACHE_LRU_LIST)
			d_lru_del(dentry);
		if (!dentry->d_lockref.count) {
			d_shrink_add(dentry, &data->dispose);
			data->found++;
		}
	}
	/*
	 * We can return to the caller if we have found some (this
	 * ensures forward progress). We'll be coming back to find
	 * the rest.
	 */
	if (!list_empty(&data->dispose))
		ret = need_resched() ? D_WALK_QUIT : D_WALK_NORETRY;
out:
	return ret;
}
