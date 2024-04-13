static void vhost_scsi_exit(void)
{
	vhost_scsi_deregister_configfs();
	vhost_scsi_deregister();
	destroy_workqueue(vhost_scsi_workqueue);
};
