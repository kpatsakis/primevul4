static int __init vhost_scsi_init(void)
{
	int ret = -ENOMEM;
	/*
	 * Use our own dedicated workqueue for submitting I/O into
	 * target core to avoid contention within system_wq.
	 */
	vhost_scsi_workqueue = alloc_workqueue("vhost_scsi", 0, 0);
	if (!vhost_scsi_workqueue)
		goto out;

	ret = vhost_scsi_register();
	if (ret < 0)
		goto out_destroy_workqueue;

	ret = vhost_scsi_register_configfs();
	if (ret < 0)
		goto out_vhost_scsi_deregister;

	return 0;

out_vhost_scsi_deregister:
	vhost_scsi_deregister();
out_destroy_workqueue:
	destroy_workqueue(vhost_scsi_workqueue);
out:
	return ret;
};
