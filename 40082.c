static int __init ax25_init(void)
{
	int rc = proto_register(&ax25_proto, 0);

	if (rc != 0)
		goto out;

	sock_register(&ax25_family_ops);
	dev_add_pack(&ax25_packet_type);
	register_netdevice_notifier(&ax25_dev_notifier);

	proc_create("ax25_route", S_IRUGO, init_net.proc_net,
		    &ax25_route_fops);
	proc_create("ax25", S_IRUGO, init_net.proc_net, &ax25_info_fops);
	proc_create("ax25_calls", S_IRUGO, init_net.proc_net, &ax25_uid_fops);
out:
	return rc;
}
