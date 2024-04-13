static int __init sock_init(void)
{
	int err;
	/*
	 *      Initialize the network sysctl infrastructure.
	 */
	err = net_sysctl_init();
	if (err)
		goto out;

	/*
	 *      Initialize skbuff SLAB cache
	 */
	skb_init();

	/*
	 *      Initialize the protocols module.
	 */

	init_inodecache();

	err = register_filesystem(&sock_fs_type);
	if (err)
		goto out_fs;
	sock_mnt = kern_mount(&sock_fs_type);
	if (IS_ERR(sock_mnt)) {
		err = PTR_ERR(sock_mnt);
		goto out_mount;
	}

	/* The real protocol initialization is performed in later initcalls.
	 */

#ifdef CONFIG_NETFILTER
	err = netfilter_init();
	if (err)
		goto out;
#endif

#ifdef CONFIG_NETWORK_PHY_TIMESTAMPING
	skb_timestamping_init();
#endif

out:
	return err;

out_mount:
	unregister_filesystem(&sock_fs_type);
out_fs:
	goto out;
}
