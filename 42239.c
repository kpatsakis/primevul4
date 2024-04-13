static int vq_log_access_ok(struct vhost_dev *d, struct vhost_virtqueue *vq,
			    void __user *log_base)
{
	struct vhost_memory *mp;
	size_t s = vhost_has_feature(d, VIRTIO_RING_F_EVENT_IDX) ? 2 : 0;

	mp = rcu_dereference_protected(vq->dev->memory,
				       lockdep_is_held(&vq->mutex));
	return vq_memory_access_ok(log_base, mp,
			    vhost_has_feature(vq->dev, VHOST_F_LOG_ALL)) &&
		(!vq->log_used || log_access_ok(log_base, vq->log_addr,
					sizeof *vq->used +
					vq->num * sizeof *vq->used->ring + s));
}
