static int __init crypto_user_init(void)
{
	struct netlink_kernel_cfg cfg = {
		.input	= crypto_netlink_rcv,
	};

	crypto_nlsk = netlink_kernel_create(&init_net, NETLINK_CRYPTO, &cfg);
	if (!crypto_nlsk)
		return -ENOMEM;

	return 0;
}
