static void __exit sclp_ctl_exit(void)
{
	misc_deregister(&sclp_ctl_device);
}
