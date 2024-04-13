static int bond_ioctl(struct net *net, unsigned int cmd,
			 struct compat_ifreq __user *ifr32)
{
	struct ifreq kifr;
	mm_segment_t old_fs;
	int err;

	switch (cmd) {
	case SIOCBONDENSLAVE:
	case SIOCBONDRELEASE:
	case SIOCBONDSETHWADDR:
	case SIOCBONDCHANGEACTIVE:
		if (copy_from_user(&kifr, ifr32, sizeof(struct compat_ifreq)))
			return -EFAULT;

		old_fs = get_fs();
		set_fs(KERNEL_DS);
		err = dev_ioctl(net, cmd,
				(struct ifreq __user __force *) &kifr);
		set_fs(old_fs);

		return err;
	default:
		return -ENOIOCTLCMD;
	}
}
