static int vq2txq(struct virtqueue *vq)
{
	return (vq->index - 1) / 2;
}
