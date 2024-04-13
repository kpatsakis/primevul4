static int vhost_scsi_deregister(void)
{
	return misc_deregister(&vhost_scsi_misc);
}
