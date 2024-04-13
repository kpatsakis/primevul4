static int ethtool_ioctl(struct net *net, struct compat_ifreq __user *ifr32)
{
	struct compat_ethtool_rxnfc __user *compat_rxnfc;
	bool convert_in = false, convert_out = false;
	size_t buf_size = ALIGN(sizeof(struct ifreq), 8);
	struct ethtool_rxnfc __user *rxnfc;
	struct ifreq __user *ifr;
	u32 rule_cnt = 0, actual_rule_cnt;
	u32 ethcmd;
	u32 data;
	int ret;

	if (get_user(data, &ifr32->ifr_ifru.ifru_data))
		return -EFAULT;

	compat_rxnfc = compat_ptr(data);

	if (get_user(ethcmd, &compat_rxnfc->cmd))
		return -EFAULT;

	/* Most ethtool structures are defined without padding.
	 * Unfortunately struct ethtool_rxnfc is an exception.
	 */
	switch (ethcmd) {
	default:
		break;
	case ETHTOOL_GRXCLSRLALL:
		/* Buffer size is variable */
		if (get_user(rule_cnt, &compat_rxnfc->rule_cnt))
			return -EFAULT;
		if (rule_cnt > KMALLOC_MAX_SIZE / sizeof(u32))
			return -ENOMEM;
		buf_size += rule_cnt * sizeof(u32);
		/* fall through */
	case ETHTOOL_GRXRINGS:
	case ETHTOOL_GRXCLSRLCNT:
	case ETHTOOL_GRXCLSRULE:
	case ETHTOOL_SRXCLSRLINS:
		convert_out = true;
		/* fall through */
	case ETHTOOL_SRXCLSRLDEL:
		buf_size += sizeof(struct ethtool_rxnfc);
		convert_in = true;
		break;
	}

	ifr = compat_alloc_user_space(buf_size);
	rxnfc = (void __user *)ifr + ALIGN(sizeof(struct ifreq), 8);

	if (copy_in_user(&ifr->ifr_name, &ifr32->ifr_name, IFNAMSIZ))
		return -EFAULT;

	if (put_user(convert_in ? rxnfc : compat_ptr(data),
		     &ifr->ifr_ifru.ifru_data))
		return -EFAULT;

	if (convert_in) {
		/* We expect there to be holes between fs.m_ext and
		 * fs.ring_cookie and at the end of fs, but nowhere else.
		 */
		BUILD_BUG_ON(offsetof(struct compat_ethtool_rxnfc, fs.m_ext) +
			     sizeof(compat_rxnfc->fs.m_ext) !=
			     offsetof(struct ethtool_rxnfc, fs.m_ext) +
			     sizeof(rxnfc->fs.m_ext));
		BUILD_BUG_ON(
			offsetof(struct compat_ethtool_rxnfc, fs.location) -
			offsetof(struct compat_ethtool_rxnfc, fs.ring_cookie) !=
			offsetof(struct ethtool_rxnfc, fs.location) -
			offsetof(struct ethtool_rxnfc, fs.ring_cookie));

		if (copy_in_user(rxnfc, compat_rxnfc,
				 (void __user *)(&rxnfc->fs.m_ext + 1) -
				 (void __user *)rxnfc) ||
		    copy_in_user(&rxnfc->fs.ring_cookie,
				 &compat_rxnfc->fs.ring_cookie,
				 (void __user *)(&rxnfc->fs.location + 1) -
				 (void __user *)&rxnfc->fs.ring_cookie) ||
		    copy_in_user(&rxnfc->rule_cnt, &compat_rxnfc->rule_cnt,
				 sizeof(rxnfc->rule_cnt)))
			return -EFAULT;
	}

	ret = dev_ioctl(net, SIOCETHTOOL, ifr);
	if (ret)
		return ret;

	if (convert_out) {
		if (copy_in_user(compat_rxnfc, rxnfc,
				 (const void __user *)(&rxnfc->fs.m_ext + 1) -
				 (const void __user *)rxnfc) ||
		    copy_in_user(&compat_rxnfc->fs.ring_cookie,
				 &rxnfc->fs.ring_cookie,
				 (const void __user *)(&rxnfc->fs.location + 1) -
				 (const void __user *)&rxnfc->fs.ring_cookie) ||
		    copy_in_user(&compat_rxnfc->rule_cnt, &rxnfc->rule_cnt,
				 sizeof(rxnfc->rule_cnt)))
			return -EFAULT;

		if (ethcmd == ETHTOOL_GRXCLSRLALL) {
			/* As an optimisation, we only copy the actual
			 * number of rules that the underlying
			 * function returned.  Since Mallory might
			 * change the rule count in user memory, we
			 * check that it is less than the rule count
			 * originally given (as the user buffer size),
			 * which has been range-checked.
			 */
			if (get_user(actual_rule_cnt, &rxnfc->rule_cnt))
				return -EFAULT;
			if (actual_rule_cnt < rule_cnt)
				rule_cnt = actual_rule_cnt;
			if (copy_in_user(&compat_rxnfc->rule_locs[0],
					 &rxnfc->rule_locs[0],
					 rule_cnt * sizeof(u32)))
				return -EFAULT;
		}
	}

	return 0;
}
