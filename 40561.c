static void __exit ipsec_pfkey_exit(void)
{
	xfrm_unregister_km(&pfkeyv2_mgr);
	sock_unregister(PF_KEY);
	unregister_pernet_subsys(&pfkey_net_ops);
	proto_unregister(&key_proto);
}
