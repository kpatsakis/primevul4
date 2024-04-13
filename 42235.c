void vhost_zerocopy_callback(struct ubuf_info *ubuf)
{
	struct vhost_ubuf_ref *ubufs = ubuf->ctx;
	struct vhost_virtqueue *vq = ubufs->vq;

	vhost_poll_queue(&vq->poll);
	/* set len = 1 to mark this desc buffers done DMA */
	vq->heads[ubuf->desc].len = VHOST_DMA_DONE_LEN;
	kref_put(&ubufs->kref, vhost_zerocopy_done_signal);
}
