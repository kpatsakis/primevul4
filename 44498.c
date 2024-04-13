static __net_init int dccp_net_init(struct net *net)
{
	int ret = 0;
	ret = nf_ct_l4proto_pernet_register(net, &dccp_proto4);
	if (ret < 0) {
		pr_err("nf_conntrack_dccp4: pernet registration failed.\n");
		goto out;
	}
	ret = nf_ct_l4proto_pernet_register(net, &dccp_proto6);
	if (ret < 0) {
		pr_err("nf_conntrack_dccp6: pernet registration failed.\n");
		goto cleanup_dccp4;
	}
	return 0;
cleanup_dccp4:
	nf_ct_l4proto_pernet_unregister(net, &dccp_proto4);
out:
	return ret;
}
