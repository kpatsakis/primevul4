static struct ipx_interface *ipxitf_auto_create(struct net_device *dev,
						__be16 dlink_type)
{
	struct ipx_interface *intrfc = NULL;
	struct datalink_proto *datalink;

	if (!dev)
		goto out;

	/* Check addresses are suitable */
	if (dev->addr_len > IPX_NODE_LEN)
		goto out;

	switch (ntohs(dlink_type)) {
	case ETH_P_IPX:		datalink = pEII_datalink;	break;
	case ETH_P_802_2:	datalink = p8022_datalink;	break;
	case ETH_P_SNAP:	datalink = pSNAP_datalink;	break;
	case ETH_P_802_3:	datalink = p8023_datalink;	break;
	default:		goto out;
	}

	intrfc = ipxitf_alloc(dev, 0, dlink_type, datalink, 0,
				dev->hard_header_len + datalink->header_length);

	if (intrfc) {
		memset(intrfc->if_node, 0, IPX_NODE_LEN);
		memcpy((char *)&(intrfc->if_node[IPX_NODE_LEN-dev->addr_len]),
			dev->dev_addr, dev->addr_len);
		spin_lock_init(&intrfc->if_sklist_lock);
		atomic_set(&intrfc->refcnt, 1);
		ipxitf_insert(intrfc);
		dev_hold(dev);
	}

out:
	return intrfc;
}
