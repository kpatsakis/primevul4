static int complete_walk(struct nameidata *nd)
{
	struct dentry *dentry = nd->path.dentry;
	int status;

	if (nd->flags & LOOKUP_RCU) {
		nd->flags &= ~LOOKUP_RCU;
		if (!(nd->flags & LOOKUP_ROOT))
			nd->root.mnt = NULL;

		if (!legitimize_mnt(nd->path.mnt, nd->m_seq)) {
			rcu_read_unlock();
			return -ECHILD;
		}
		if (unlikely(!lockref_get_not_dead(&dentry->d_lockref))) {
			rcu_read_unlock();
			mntput(nd->path.mnt);
			return -ECHILD;
		}
		if (read_seqcount_retry(&dentry->d_seq, nd->seq)) {
			rcu_read_unlock();
			dput(dentry);
			mntput(nd->path.mnt);
			return -ECHILD;
		}
		rcu_read_unlock();
	}

	if (likely(!(nd->flags & LOOKUP_JUMPED)))
		return 0;

	if (likely(!(dentry->d_flags & DCACHE_OP_WEAK_REVALIDATE)))
		return 0;

	status = dentry->d_op->d_weak_revalidate(dentry, nd->flags);
	if (status > 0)
		return 0;

	if (!status)
		status = -ESTALE;

	path_put(&nd->path);
	return status;
}
