struct kern_ipc_perm *ipc_lock(struct ipc_ids *ids, int id)
{
	struct kern_ipc_perm *out;

	rcu_read_lock();
	out = ipc_obtain_object_idr(ids, id);
	if (IS_ERR(out))
		goto err;

	spin_lock(&out->lock);

	/*
	 * ipc_rmid() may have already freed the ID while ipc_lock()
	 * was spinning: here verify that the structure is still valid.
	 * Upon races with RMID, return -EIDRM, thus indicating that
	 * the ID points to a removed identifier.
	 */
	if (ipc_valid_object(out))
		return out;

	spin_unlock(&out->lock);
	out = ERR_PTR(-EIDRM);
err:
	rcu_read_unlock();
	return out;
}
