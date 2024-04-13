static void __exit ipx_proto_finito(void)
{
	ipx_proc_exit();
	ipx_unregister_sysctl();

	unregister_netdevice_notifier(&ipx_dev_notifier);

	ipxitf_cleanup();

	if (pSNAP_datalink) {
		unregister_snap_client(pSNAP_datalink);
		pSNAP_datalink = NULL;
	}

	if (p8022_datalink) {
		unregister_8022_client(p8022_datalink);
		p8022_datalink = NULL;
	}

	dev_remove_pack(&ipx_8023_packet_type);
	if (p8023_datalink) {
		destroy_8023_client(p8023_datalink);
		p8023_datalink = NULL;
	}

	dev_remove_pack(&ipx_dix_packet_type);
	if (pEII_datalink) {
		destroy_EII_client(pEII_datalink);
		pEII_datalink = NULL;
	}

	proto_unregister(&ipx_proto);
	sock_unregister(ipx_family_ops.family);
}
