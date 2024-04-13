static enum lru_status dentry_lru_isolate(struct list_head *item,
		struct list_lru_one *lru, spinlock_t *lru_lock, void *arg)
{
	struct list_head *freeable = arg;
	struct dentry	*dentry = container_of(item, struct dentry, d_lru);


	/*
	 * we are inverting the lru lock/dentry->d_lock here,
	 * so use a trylock. If we fail to get the lock, just skip
	 * it
	 */
	if (!spin_trylock(&dentry->d_lock))
		return LRU_SKIP;

	/*
	 * Referenced dentries are still in use. If they have active
	 * counts, just remove them from the LRU. Otherwise give them
	 * another pass through the LRU.
	 */
	if (dentry->d_lockref.count) {
		d_lru_isolate(lru, dentry);
		spin_unlock(&dentry->d_lock);
		return LRU_REMOVED;
	}

	if (dentry->d_flags & DCACHE_REFERENCED) {
		dentry->d_flags &= ~DCACHE_REFERENCED;
		spin_unlock(&dentry->d_lock);

		/*
		 * The list move itself will be made by the common LRU code. At
		 * this point, we've dropped the dentry->d_lock but keep the
		 * lru lock. This is safe to do, since every list movement is
		 * protected by the lru lock even if both locks are held.
		 *
		 * This is guaranteed by the fact that all LRU management
		 * functions are intermediated by the LRU API calls like
		 * list_lru_add and list_lru_del. List movement in this file
		 * only ever occur through this functions or through callbacks
		 * like this one, that are called from the LRU API.
		 *
		 * The only exceptions to this are functions like
		 * shrink_dentry_list, and code that first checks for the
		 * DCACHE_SHRINK_LIST flag.  Those are guaranteed to be
		 * operating only with stack provided lists after they are
		 * properly isolated from the main list.  It is thus, always a
		 * local access.
		 */
		return LRU_ROTATE;
	}

	d_lru_shrink_move(lru, dentry, freeable);
	spin_unlock(&dentry->d_lock);

	return LRU_REMOVED;
}
