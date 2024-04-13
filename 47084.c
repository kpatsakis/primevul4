static int vhost_scsi_check_stop_free(struct se_cmd *se_cmd)
{
	return target_put_sess_cmd(se_cmd->se_sess, se_cmd);
}
