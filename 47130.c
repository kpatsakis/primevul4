static int __init vhost_scsi_register(void)
{
	return misc_register(&vhost_scsi_misc);
}
