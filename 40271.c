static int do_setvfinfo(struct net_device *dev, struct nlattr *attr)
{
	int rem, err = -EINVAL;
	struct nlattr *vf;
	const struct net_device_ops *ops = dev->netdev_ops;

	nla_for_each_nested(vf, attr, rem) {
		switch (nla_type(vf)) {
		case IFLA_VF_MAC: {
			struct ifla_vf_mac *ivm;
			ivm = nla_data(vf);
			err = -EOPNOTSUPP;
			if (ops->ndo_set_vf_mac)
				err = ops->ndo_set_vf_mac(dev, ivm->vf,
							  ivm->mac);
			break;
		}
		case IFLA_VF_VLAN: {
			struct ifla_vf_vlan *ivv;
			ivv = nla_data(vf);
			err = -EOPNOTSUPP;
			if (ops->ndo_set_vf_vlan)
				err = ops->ndo_set_vf_vlan(dev, ivv->vf,
							   ivv->vlan,
							   ivv->qos);
			break;
		}
		case IFLA_VF_TX_RATE: {
			struct ifla_vf_tx_rate *ivt;
			ivt = nla_data(vf);
			err = -EOPNOTSUPP;
			if (ops->ndo_set_vf_tx_rate)
				err = ops->ndo_set_vf_tx_rate(dev, ivt->vf,
							      ivt->rate);
			break;
		}
		case IFLA_VF_SPOOFCHK: {
			struct ifla_vf_spoofchk *ivs;
			ivs = nla_data(vf);
			err = -EOPNOTSUPP;
			if (ops->ndo_set_vf_spoofchk)
				err = ops->ndo_set_vf_spoofchk(dev, ivs->vf,
							       ivs->setting);
			break;
		}
		default:
			err = -EINVAL;
			break;
		}
		if (err)
			break;
	}
	return err;
}
