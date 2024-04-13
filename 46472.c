int vhost_vq_access_ok(struct vhost_virtqueue *vq)
{
	return vq_access_ok(vq, vq->num, vq->desc, vq->avail, vq->used) &&
		vq_log_access_ok(vq, vq->log_base);
}
