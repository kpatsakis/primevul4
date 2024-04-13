static void shrink_dentry_list(struct list_head *list)
{
	struct dentry *dentry, *parent;

	while (!list_empty(list)) {
		struct inode *inode;
		dentry = list_entry(list->prev, struct dentry, d_lru);
		spin_lock(&dentry->d_lock);
		parent = lock_parent(dentry);

		/*
		 * The dispose list is isolated and dentries are not accounted
		 * to the LRU here, so we can simply remove it from the list
		 * here regardless of whether it is referenced or not.
		 */
		d_shrink_del(dentry);

		/*
		 * We found an inuse dentry which was not removed from
		 * the LRU because of laziness during lookup. Do not free it.
		 */
		if (dentry->d_lockref.count > 0) {
			spin_unlock(&dentry->d_lock);
			if (parent)
				spin_unlock(&parent->d_lock);
			continue;
		}


		if (unlikely(dentry->d_flags & DCACHE_DENTRY_KILLED)) {
			bool can_free = dentry->d_flags & DCACHE_MAY_FREE;
			spin_unlock(&dentry->d_lock);
			if (parent)
				spin_unlock(&parent->d_lock);
			if (can_free)
				dentry_free(dentry);
			continue;
		}

		inode = dentry->d_inode;
		if (inode && unlikely(!spin_trylock(&inode->i_lock))) {
			d_shrink_add(dentry, list);
			spin_unlock(&dentry->d_lock);
			if (parent)
				spin_unlock(&parent->d_lock);
			continue;
		}

		__dentry_kill(dentry);

		/*
		 * We need to prune ancestors too. This is necessary to prevent
		 * quadratic behavior of shrink_dcache_parent(), but is also
		 * expected to be beneficial in reducing dentry cache
		 * fragmentation.
		 */
		dentry = parent;
		while (dentry && !lockref_put_or_lock(&dentry->d_lockref)) {
			parent = lock_parent(dentry);
			if (dentry->d_lockref.count != 1) {
				dentry->d_lockref.count--;
				spin_unlock(&dentry->d_lock);
				if (parent)
					spin_unlock(&parent->d_lock);
				break;
			}
			inode = dentry->d_inode;	/* can't be NULL */
			if (unlikely(!spin_trylock(&inode->i_lock))) {
				spin_unlock(&dentry->d_lock);
				if (parent)
					spin_unlock(&parent->d_lock);
				cpu_relax();
				continue;
			}
			__dentry_kill(dentry);
			dentry = parent;
		}
	}
}
