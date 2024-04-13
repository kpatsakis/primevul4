static inline bool fast_dput(struct dentry *dentry)
{
	int ret;
	unsigned int d_flags;

	/*
	 * If we have a d_op->d_delete() operation, we sould not
	 * let the dentry count go to zero, so use "put_or_lock".
	 */
	if (unlikely(dentry->d_flags & DCACHE_OP_DELETE))
		return lockref_put_or_lock(&dentry->d_lockref);

	/*
	 * .. otherwise, we can try to just decrement the
	 * lockref optimistically.
	 */
	ret = lockref_put_return(&dentry->d_lockref);

	/*
	 * If the lockref_put_return() failed due to the lock being held
	 * by somebody else, the fast path has failed. We will need to
	 * get the lock, and then check the count again.
	 */
	if (unlikely(ret < 0)) {
		spin_lock(&dentry->d_lock);
		if (dentry->d_lockref.count > 1) {
			dentry->d_lockref.count--;
			spin_unlock(&dentry->d_lock);
			return 1;
		}
		return 0;
	}

	/*
	 * If we weren't the last ref, we're done.
	 */
	if (ret)
		return 1;

	/*
	 * Careful, careful. The reference count went down
	 * to zero, but we don't hold the dentry lock, so
	 * somebody else could get it again, and do another
	 * dput(), and we need to not race with that.
	 *
	 * However, there is a very special and common case
	 * where we don't care, because there is nothing to
	 * do: the dentry is still hashed, it does not have
	 * a 'delete' op, and it's referenced and already on
	 * the LRU list.
	 *
	 * NOTE! Since we aren't locked, these values are
	 * not "stable". However, it is sufficient that at
	 * some point after we dropped the reference the
	 * dentry was hashed and the flags had the proper
	 * value. Other dentry users may have re-gotten
	 * a reference to the dentry and change that, but
	 * our work is done - we can leave the dentry
	 * around with a zero refcount.
	 */
	smp_rmb();
	d_flags = ACCESS_ONCE(dentry->d_flags);
	d_flags &= DCACHE_REFERENCED | DCACHE_LRU_LIST | DCACHE_DISCONNECTED;

	/* Nothing to do? Dropping the reference was all we needed? */
	if (d_flags == (DCACHE_REFERENCED | DCACHE_LRU_LIST) && !d_unhashed(dentry))
		return 1;

	/*
	 * Not the fast normal case? Get the lock. We've already decremented
	 * the refcount, but we'll need to re-check the situation after
	 * getting the lock.
	 */
	spin_lock(&dentry->d_lock);

	/*
	 * Did somebody else grab a reference to it in the meantime, and
	 * we're no longer the last user after all? Alternatively, somebody
	 * else could have killed it and marked it dead. Either way, we
	 * don't need to do anything else.
	 */
	if (dentry->d_lockref.count) {
		spin_unlock(&dentry->d_lock);
		return 1;
	}

	/*
	 * Re-get the reference we optimistically dropped. We hold the
	 * lock, and we just tested that it was zero, so we can just
	 * set it to 1.
	 */
	dentry->d_lockref.count = 1;
	return 0;
}
