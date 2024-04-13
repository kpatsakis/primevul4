static long vhost_set_memory(struct vhost_dev *d, struct vhost_memory __user *m)
{
	struct vhost_memory mem, *newmem, *oldmem;
	unsigned long size = offsetof(struct vhost_memory, regions);

	if (copy_from_user(&mem, m, size))
		return -EFAULT;
	if (mem.padding)
		return -EOPNOTSUPP;
	if (mem.nregions > VHOST_MEMORY_MAX_NREGIONS)
		return -E2BIG;
	newmem = kmalloc(size + mem.nregions * sizeof *m->regions, GFP_KERNEL);
	if (!newmem)
		return -ENOMEM;

	memcpy(newmem, &mem, size);
	if (copy_from_user(newmem->regions, m->regions,
			   mem.nregions * sizeof *m->regions)) {
		kfree(newmem);
		return -EFAULT;
	}

	if (!memory_access_ok(d, newmem,
			      vhost_has_feature(d, VHOST_F_LOG_ALL))) {
		kfree(newmem);
		return -EFAULT;
	}
	oldmem = rcu_dereference_protected(d->memory,
					   lockdep_is_held(&d->mutex));
	rcu_assign_pointer(d->memory, newmem);
	synchronize_rcu();
	kfree(oldmem);
	return 0;
}
