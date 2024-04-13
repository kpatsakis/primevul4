vhost_scsi_mapal(struct vhost_scsi_cmd *cmd,
		 size_t prot_bytes, struct iov_iter *prot_iter,
		 size_t data_bytes, struct iov_iter *data_iter)
{
	int sgl_count, ret;
	bool write = (cmd->tvc_data_direction == DMA_FROM_DEVICE);

	if (prot_bytes) {
		sgl_count = vhost_scsi_calc_sgls(prot_iter, prot_bytes,
						 VHOST_SCSI_PREALLOC_PROT_SGLS);
		if (sgl_count < 0)
			return sgl_count;

		sg_init_table(cmd->tvc_prot_sgl, sgl_count);
		cmd->tvc_prot_sgl_count = sgl_count;
		pr_debug("%s prot_sg %p prot_sgl_count %u\n", __func__,
			 cmd->tvc_prot_sgl, cmd->tvc_prot_sgl_count);

		ret = vhost_scsi_iov_to_sgl(cmd, write, prot_iter,
					    cmd->tvc_prot_sgl,
					    cmd->tvc_prot_sgl_count);
		if (ret < 0) {
			cmd->tvc_prot_sgl_count = 0;
			return ret;
		}
	}
	sgl_count = vhost_scsi_calc_sgls(data_iter, data_bytes,
					 VHOST_SCSI_PREALLOC_SGLS);
	if (sgl_count < 0)
		return sgl_count;

	sg_init_table(cmd->tvc_sgl, sgl_count);
	cmd->tvc_sgl_count = sgl_count;
	pr_debug("%s data_sg %p data_sgl_count %u\n", __func__,
		  cmd->tvc_sgl, cmd->tvc_sgl_count);

	ret = vhost_scsi_iov_to_sgl(cmd, write, data_iter,
				    cmd->tvc_sgl, cmd->tvc_sgl_count);
	if (ret < 0) {
		cmd->tvc_sgl_count = 0;
		return ret;
	}
	return 0;
}
