static int pipe_to_sg(struct pipe_inode_info *pipe, struct pipe_buffer *buf,
			struct splice_desc *sd)
{
	struct sg_list *sgl = sd->u.data;
	unsigned int offset, len;

	if (sgl->n == sgl->size)
		return 0;

	/* Try lock this page */
	if (pipe_buf_steal(pipe, buf) == 0) {
		/* Get reference and unlock page for moving */
		get_page(buf->page);
		unlock_page(buf->page);

		len = min(buf->len, sd->len);
		sg_set_page(&(sgl->sg[sgl->n]), buf->page, len, buf->offset);
	} else {
		/* Failback to copying a page */
		struct page *page = alloc_page(GFP_KERNEL);
		char *src;

		if (!page)
			return -ENOMEM;

		offset = sd->pos & ~PAGE_MASK;

		len = sd->len;
		if (len + offset > PAGE_SIZE)
			len = PAGE_SIZE - offset;

		src = kmap_atomic(buf->page);
		memcpy(page_address(page) + offset, src + buf->offset, len);
		kunmap_atomic(src);

		sg_set_page(&(sgl->sg[sgl->n]), page, len, offset);
	}
	sgl->n++;
	sgl->len += len;

	return len;
}
