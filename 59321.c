static int instantiate_veth(char *n1, char **n2)
{
	int err;

	err = snprintf(*n2, IFNAMSIZ, "%sp", n1);
	if (err < 0 || err >= IFNAMSIZ) {
		fprintf(stderr, "nic name too long\n");
		return -1;
	}

	err = lxc_veth_create(n1, *n2);
	if (err) {
		fprintf(stderr, "failed to create %s-%s : %s\n", n1, *n2,
		      strerror(-err));
		return -1;
	}

	/* changing the high byte of the mac address to 0xfe, the bridge interface
	 * will always keep the host's mac address and not take the mac address
	 * of a container */
	err = setup_private_host_hw_addr(n1);
	if (err) {
		fprintf(stderr, "failed to change mac address of host interface '%s' : %s\n",
			n1, strerror(-err));
	}

	return netdev_set_flag(n1, IFF_UP);
}
