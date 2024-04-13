static int vq_log_access_ok(struct vhost_virtqueue *vq,
			    void __user *log_base)
{
	size_t s = vhost_has_feature(vq, VIRTIO_RING_F_EVENT_IDX) ? 2 : 0;

	return vq_memory_access_ok(log_base, vq->memory,
				   vhost_has_feature(vq, VHOST_F_LOG_ALL)) &&
		(!vq->log_used || log_access_ok(log_base, vq->log_addr,
					sizeof *vq->used +
					vq->num * sizeof *vq->used->ring + s));
}
