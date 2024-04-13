static int memory_access_ok(struct vhost_dev *d, struct vhost_memory *mem,
			    int log_all)
{
	int i;

	for (i = 0; i < d->nvqs; ++i) {
		int ok;
		bool log;

		mutex_lock(&d->vqs[i]->mutex);
		log = log_all || vhost_has_feature(d->vqs[i], VHOST_F_LOG_ALL);
		/* If ring is inactive, will check when it's enabled. */
		if (d->vqs[i]->private_data)
			ok = vq_memory_access_ok(d->vqs[i]->log_base, mem, log);
		else
			ok = 1;
		mutex_unlock(&d->vqs[i]->mutex);
		if (!ok)
			return 0;
	}
	return 1;
}
