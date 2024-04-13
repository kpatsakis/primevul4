static void vhost_scsi_complete_cmd(struct vhost_scsi_cmd *cmd)
{
	struct vhost_scsi *vs = cmd->tvc_vhost;

	llist_add(&cmd->tvc_completion_list, &vs->vs_completion_list);

	vhost_work_queue(&vs->dev, &vs->vs_completion_work);
}
