static int vq2rxq(struct virtqueue *vq)
{
	return vq->index / 2;
}
