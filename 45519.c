struct ipx_interface *ipxitf_find_using_net(__be32 net)
{
	struct ipx_interface *i;

	spin_lock_bh(&ipx_interfaces_lock);
	if (net) {
		list_for_each_entry(i, &ipx_interfaces, node)
			if (i->if_netnum == net)
				goto hold;
		i = NULL;
		goto unlock;
	}

	i = ipx_primary_net;
	if (i)
hold:
		ipxitf_hold(i);
unlock:
	spin_unlock_bh(&ipx_interfaces_lock);
	return i;
}
