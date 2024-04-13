static struct socket *vhost_net_stop_vq(struct vhost_net *n,
					struct vhost_virtqueue *vq)
{
	struct socket *sock;

	mutex_lock(&vq->mutex);
	sock = vq->private_data;
	vhost_net_disable_vq(n, vq);
	vq->private_data = NULL;
	mutex_unlock(&vq->mutex);
	return sock;
}
