static long vhost_set_memory(struct vhost_dev *d, struct vhost_memory __user *m)
{
	struct vhost_memory mem, *newmem, *oldmem;
	unsigned long size = offsetof(struct vhost_memory, regions);
	int i;

	if (copy_from_user(&mem, m, size))
		return -EFAULT;
	if (mem.padding)
		return -EOPNOTSUPP;
	if (mem.nregions > max_mem_regions)
		return -E2BIG;
	newmem = vhost_kvzalloc(size + mem.nregions * sizeof(*m->regions));
	if (!newmem)
		return -ENOMEM;

	memcpy(newmem, &mem, size);
	if (copy_from_user(newmem->regions, m->regions,
			   mem.nregions * sizeof *m->regions)) {
		kvfree(newmem);
		return -EFAULT;
	}
	sort(newmem->regions, newmem->nregions, sizeof(*newmem->regions),
		vhost_memory_reg_sort_cmp, NULL);

	if (!memory_access_ok(d, newmem, 0)) {
		kvfree(newmem);
		return -EFAULT;
	}
	oldmem = d->memory;
	d->memory = newmem;

	/* All memory accesses are done under some VQ mutex. */
	for (i = 0; i < d->nvqs; ++i) {
		mutex_lock(&d->vqs[i]->mutex);
		d->vqs[i]->memory = newmem;
		mutex_unlock(&d->vqs[i]->mutex);
	}
	kvfree(oldmem);
	return 0;
}
