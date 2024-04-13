static int ipxitf_create(struct ipx_interface_definition *idef)
{
	struct net_device *dev;
	__be16 dlink_type = 0;
	struct datalink_proto *datalink = NULL;
	struct ipx_interface *intrfc;
	int rc;

	if (idef->ipx_special == IPX_INTERNAL) {
		rc = ipxitf_create_internal(idef);
		goto out;
	}

	rc = -EEXIST;
	if (idef->ipx_special == IPX_PRIMARY && ipx_primary_net)
		goto out;

	intrfc = ipxitf_find_using_net(idef->ipx_network);
	rc = -EADDRINUSE;
	if (idef->ipx_network && intrfc) {
		ipxitf_put(intrfc);
		goto out;
	}

	if (intrfc)
		ipxitf_put(intrfc);

	dev = dev_get_by_name(&init_net, idef->ipx_device);
	rc = -ENODEV;
	if (!dev)
		goto out;

	switch (idef->ipx_dlink_type) {
	case IPX_FRAME_8022:
		dlink_type 	= htons(ETH_P_802_2);
		datalink 	= p8022_datalink;
		break;
	case IPX_FRAME_ETHERII:
		if (dev->type != ARPHRD_IEEE802) {
			dlink_type 	= htons(ETH_P_IPX);
			datalink 	= pEII_datalink;
			break;
		}
		/* fall through */
	case IPX_FRAME_SNAP:
		dlink_type 	= htons(ETH_P_SNAP);
		datalink 	= pSNAP_datalink;
		break;
	case IPX_FRAME_8023:
		dlink_type 	= htons(ETH_P_802_3);
		datalink 	= p8023_datalink;
		break;
	case IPX_FRAME_NONE:
	default:
		rc = -EPROTONOSUPPORT;
		goto out_dev;
	}

	rc = -ENETDOWN;
	if (!(dev->flags & IFF_UP))
		goto out_dev;

	/* Check addresses are suitable */
	rc = -EINVAL;
	if (dev->addr_len > IPX_NODE_LEN)
		goto out_dev;

	intrfc = ipxitf_find_using_phys(dev, dlink_type);
	if (!intrfc) {
		/* Ok now create */
		intrfc = ipxitf_alloc(dev, idef->ipx_network, dlink_type,
				      datalink, 0, dev->hard_header_len +
					datalink->header_length);
		rc = -EAGAIN;
		if (!intrfc)
			goto out_dev;
		/* Setup primary if necessary */
		if (idef->ipx_special == IPX_PRIMARY)
			ipx_primary_net = intrfc;
		if (!memcmp(idef->ipx_node, "\000\000\000\000\000\000",
			    IPX_NODE_LEN)) {
			memset(intrfc->if_node, 0, IPX_NODE_LEN);
			memcpy(intrfc->if_node + IPX_NODE_LEN - dev->addr_len,
				dev->dev_addr, dev->addr_len);
		} else
			memcpy(intrfc->if_node, idef->ipx_node, IPX_NODE_LEN);
		ipxitf_hold(intrfc);
		ipxitf_insert(intrfc);
	}


	/* If the network number is known, add a route */
	rc = 0;
	if (!intrfc->if_netnum)
		goto out_intrfc;

	rc = ipxitf_add_local_route(intrfc);
out_intrfc:
	ipxitf_put(intrfc);
	goto out;
out_dev:
	dev_put(dev);
out:
	return rc;
}
