static int ipxitf_create_internal(struct ipx_interface_definition *idef)
{
	struct ipx_interface *intrfc;
	int rc = -EEXIST;

	/* Only one primary network allowed */
	if (ipx_primary_net)
		goto out;

	/* Must have a valid network number */
	rc = -EADDRNOTAVAIL;
	if (!idef->ipx_network)
		goto out;
	intrfc = ipxitf_find_using_net(idef->ipx_network);
	rc = -EADDRINUSE;
	if (intrfc) {
		ipxitf_put(intrfc);
		goto out;
	}
	intrfc = ipxitf_alloc(NULL, idef->ipx_network, 0, NULL, 1, 0);
	rc = -EAGAIN;
	if (!intrfc)
		goto out;
	memcpy((char *)&(intrfc->if_node), idef->ipx_node, IPX_NODE_LEN);
	ipx_internal_net = ipx_primary_net = intrfc;
	ipxitf_hold(intrfc);
	ipxitf_insert(intrfc);

	rc = ipxitf_add_local_route(intrfc);
	ipxitf_put(intrfc);
out:
	return rc;
}
