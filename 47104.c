static void vhost_scsi_free_cmd_map_res(struct vhost_scsi_nexus *nexus,
				       struct se_session *se_sess)
{
	struct vhost_scsi_cmd *tv_cmd;
	unsigned int i;

	if (!se_sess->sess_cmd_map)
		return;

	for (i = 0; i < VHOST_SCSI_DEFAULT_TAGS; i++) {
		tv_cmd = &((struct vhost_scsi_cmd *)se_sess->sess_cmd_map)[i];

		kfree(tv_cmd->tvc_sgl);
		kfree(tv_cmd->tvc_prot_sgl);
		kfree(tv_cmd->tvc_upages);
	}
}
