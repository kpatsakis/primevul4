static void vhost_scsi_release_cmd(struct se_cmd *se_cmd)
{
	struct vhost_scsi_cmd *tv_cmd = container_of(se_cmd,
				struct vhost_scsi_cmd, tvc_se_cmd);
	struct se_session *se_sess = tv_cmd->tvc_nexus->tvn_se_sess;
	int i;

	if (tv_cmd->tvc_sgl_count) {
		for (i = 0; i < tv_cmd->tvc_sgl_count; i++)
			put_page(sg_page(&tv_cmd->tvc_sgl[i]));
	}
	if (tv_cmd->tvc_prot_sgl_count) {
		for (i = 0; i < tv_cmd->tvc_prot_sgl_count; i++)
			put_page(sg_page(&tv_cmd->tvc_prot_sgl[i]));
	}

	vhost_scsi_put_inflight(tv_cmd->inflight);
	percpu_ida_free(&se_sess->sess_tag_pool, se_cmd->map_tag);
}
