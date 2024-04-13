static inline int rtnl_vfinfo_size(const struct net_device *dev,
				   u32 ext_filter_mask)
{
	if (dev->dev.parent && dev_is_pci(dev->dev.parent) &&
	    (ext_filter_mask & RTEXT_FILTER_VF)) {
		int num_vfs = dev_num_vf(dev->dev.parent);
		size_t size = nla_total_size(sizeof(struct nlattr));
		size += nla_total_size(num_vfs * sizeof(struct nlattr));
		size += num_vfs *
			(nla_total_size(sizeof(struct ifla_vf_mac)) +
			 nla_total_size(sizeof(struct ifla_vf_vlan)) +
			 nla_total_size(sizeof(struct ifla_vf_tx_rate)) +
			 nla_total_size(sizeof(struct ifla_vf_spoofchk)));
		return size;
	} else
		return 0;
}
