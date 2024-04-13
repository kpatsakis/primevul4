vhost_scsi_iov_to_sgl(struct vhost_scsi_cmd *cmd, bool write,
		      struct iov_iter *iter,
		      struct scatterlist *sg, int sg_count)
{
	size_t off = iter->iov_offset;
	int i, ret;

	for (i = 0; i < iter->nr_segs; i++) {
		void __user *base = iter->iov[i].iov_base + off;
		size_t len = iter->iov[i].iov_len - off;

		ret = vhost_scsi_map_to_sgl(cmd, base, len, sg, write);
		if (ret < 0) {
			for (i = 0; i < sg_count; i++) {
				struct page *page = sg_page(&sg[i]);
				if (page)
					put_page(page);
			}
			return ret;
		}
		sg += ret;
		off = 0;
	}
	return 0;
}
