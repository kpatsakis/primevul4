static void vhost_scsi_deregister_configfs(void)
{
	if (!vhost_scsi_fabric_configfs)
		return;

	target_fabric_configfs_deregister(vhost_scsi_fabric_configfs);
	vhost_scsi_fabric_configfs = NULL;
	pr_debug("TCM_VHOST[0] - Cleared vhost_scsi_fabric_configfs\n");
};
