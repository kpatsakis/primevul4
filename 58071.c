static struct port_buffer *alloc_buf(struct virtqueue *vq, size_t buf_size,
				     int pages)
{
	struct port_buffer *buf;

	reclaim_dma_bufs();

	/*
	 * Allocate buffer and the sg list. The sg list array is allocated
	 * directly after the port_buffer struct.
	 */
	buf = kmalloc(sizeof(*buf) + sizeof(struct scatterlist) * pages,
		      GFP_KERNEL);
	if (!buf)
		goto fail;

	buf->sgpages = pages;
	if (pages > 0) {
		buf->dev = NULL;
		buf->buf = NULL;
		return buf;
	}

	if (is_rproc_serial(vq->vdev)) {
		/*
		 * Allocate DMA memory from ancestor. When a virtio
		 * device is created by remoteproc, the DMA memory is
		 * associated with the grandparent device:
		 * vdev => rproc => platform-dev.
		 * The code here would have been less quirky if
		 * DMA_MEMORY_INCLUDES_CHILDREN had been supported
		 * in dma-coherent.c
		 */
		if (!vq->vdev->dev.parent || !vq->vdev->dev.parent->parent)
			goto free_buf;
		buf->dev = vq->vdev->dev.parent->parent;

		/* Increase device refcnt to avoid freeing it */
		get_device(buf->dev);
		buf->buf = dma_alloc_coherent(buf->dev, buf_size, &buf->dma,
					      GFP_KERNEL);
	} else {
		buf->dev = NULL;
		buf->buf = kmalloc(buf_size, GFP_KERNEL);
	}

	if (!buf->buf)
		goto free_buf;
	buf->len = 0;
	buf->offset = 0;
	buf->size = buf_size;
	return buf;

free_buf:
	kfree(buf);
fail:
	return NULL;
}
