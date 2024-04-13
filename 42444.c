void ip_vs_unregister_nl_ioctl(void)
{
	ip_vs_genl_unregister();
	nf_unregister_sockopt(&ip_vs_sockopts);
}
