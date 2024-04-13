static void vhost_scsi_submission_work(struct work_struct *work)
{
	struct vhost_scsi_cmd *cmd =
		container_of(work, struct vhost_scsi_cmd, work);
	struct vhost_scsi_nexus *tv_nexus;
	struct se_cmd *se_cmd = &cmd->tvc_se_cmd;
	struct scatterlist *sg_ptr, *sg_prot_ptr = NULL;
	int rc;

	/* FIXME: BIDI operation */
	if (cmd->tvc_sgl_count) {
		sg_ptr = cmd->tvc_sgl;

		if (cmd->tvc_prot_sgl_count)
			sg_prot_ptr = cmd->tvc_prot_sgl;
		else
			se_cmd->prot_pto = true;
	} else {
		sg_ptr = NULL;
	}
	tv_nexus = cmd->tvc_nexus;

	rc = target_submit_cmd_map_sgls(se_cmd, tv_nexus->tvn_se_sess,
			cmd->tvc_cdb, &cmd->tvc_sense_buf[0],
			cmd->tvc_lun, cmd->tvc_exp_data_len,
			cmd->tvc_task_attr, cmd->tvc_data_direction,
			TARGET_SCF_ACK_KREF, sg_ptr, cmd->tvc_sgl_count,
			NULL, 0, sg_prot_ptr, cmd->tvc_prot_sgl_count);
	if (rc < 0) {
		transport_send_check_condition_and_sense(se_cmd,
				TCM_LOGICAL_UNIT_COMMUNICATION_FAILURE, 0);
		transport_generic_free_cmd(se_cmd, 0);
	}
}
