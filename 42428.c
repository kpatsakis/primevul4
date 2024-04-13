int __init ip_vs_register_nl_ioctl(void)
{
	int ret;

	ret = nf_register_sockopt(&ip_vs_sockopts);
	if (ret) {
		pr_err("cannot register sockopt.\n");
		goto err_sock;
	}

	ret = ip_vs_genl_register();
	if (ret) {
		pr_err("cannot register Generic Netlink interface.\n");
		goto err_genl;
	}
	return 0;

err_genl:
	nf_unregister_sockopt(&ip_vs_sockopts);
err_sock:
	return ret;
}
