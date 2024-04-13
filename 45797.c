static int __init x25_init(void)
{
	int rc = proto_register(&x25_proto, 0);

	if (rc != 0)
		goto out;

	rc = sock_register(&x25_family_ops);
	if (rc != 0)
		goto out_proto;

	dev_add_pack(&x25_packet_type);

	rc = register_netdevice_notifier(&x25_dev_notifier);
	if (rc != 0)
		goto out_sock;

	printk(KERN_INFO "X.25 for Linux Version 0.2\n");

	x25_register_sysctl();
	rc = x25_proc_init();
	if (rc != 0)
		goto out_dev;
out:
	return rc;
out_dev:
	unregister_netdevice_notifier(&x25_dev_notifier);
out_sock:
	sock_unregister(AF_X25);
out_proto:
	proto_unregister(&x25_proto);
	goto out;
}
