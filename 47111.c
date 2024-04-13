vhost_scsi_get_tag(struct vhost_virtqueue *vq, struct vhost_scsi_tpg *tpg,
		   unsigned char *cdb, u64 scsi_tag, u16 lun, u8 task_attr,
		   u32 exp_data_len, int data_direction)
{
	struct vhost_scsi_cmd *cmd;
	struct vhost_scsi_nexus *tv_nexus;
	struct se_session *se_sess;
	struct scatterlist *sg, *prot_sg;
	struct page **pages;
	int tag;

	tv_nexus = tpg->tpg_nexus;
	if (!tv_nexus) {
		pr_err("Unable to locate active struct vhost_scsi_nexus\n");
		return ERR_PTR(-EIO);
	}
	se_sess = tv_nexus->tvn_se_sess;

	tag = percpu_ida_alloc(&se_sess->sess_tag_pool, TASK_RUNNING);
	if (tag < 0) {
		pr_err("Unable to obtain tag for vhost_scsi_cmd\n");
		return ERR_PTR(-ENOMEM);
	}

	cmd = &((struct vhost_scsi_cmd *)se_sess->sess_cmd_map)[tag];
	sg = cmd->tvc_sgl;
	prot_sg = cmd->tvc_prot_sgl;
	pages = cmd->tvc_upages;
	memset(cmd, 0, sizeof(struct vhost_scsi_cmd));

	cmd->tvc_sgl = sg;
	cmd->tvc_prot_sgl = prot_sg;
	cmd->tvc_upages = pages;
	cmd->tvc_se_cmd.map_tag = tag;
	cmd->tvc_tag = scsi_tag;
	cmd->tvc_lun = lun;
	cmd->tvc_task_attr = task_attr;
	cmd->tvc_exp_data_len = exp_data_len;
	cmd->tvc_data_direction = data_direction;
	cmd->tvc_nexus = tv_nexus;
	cmd->inflight = vhost_scsi_get_inflight(vq);

	memcpy(cmd->tvc_cdb, cdb, VHOST_SCSI_MAX_CDB_SIZE);

	return cmd;
}
