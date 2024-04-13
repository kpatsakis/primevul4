static int __init sclp_ctl_init(void)
{
	return misc_register(&sclp_ctl_device);
}
