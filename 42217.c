int vhost_log_access_ok(struct vhost_dev *dev)
{
	struct vhost_memory *mp;

	mp = rcu_dereference_protected(dev->memory,
				       lockdep_is_held(&dev->mutex));
	return memory_access_ok(dev, mp, 1);
}
