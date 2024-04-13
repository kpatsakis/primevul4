struct ion_handle *ion_import_dma_buf_fd(struct ion_client *client, int fd)
{
	struct dma_buf *dmabuf;
	struct ion_handle *handle;

	dmabuf = dma_buf_get(fd);
	if (IS_ERR(dmabuf))
		return ERR_CAST(dmabuf);

	handle = ion_import_dma_buf(client, dmabuf);
	dma_buf_put(dmabuf);
	return handle;
}
