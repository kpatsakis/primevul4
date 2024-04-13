static void __exit atm_exit(void)
{
	atm_proc_exit();
	atm_sysfs_exit();
	atmsvc_exit();
	atmpvc_exit();
	proto_unregister(&vcc_proto);
}
