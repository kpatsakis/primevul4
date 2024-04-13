static void ipxitf_insert(struct ipx_interface *intrfc)
{
	spin_lock_bh(&ipx_interfaces_lock);
	list_add_tail(&intrfc->node, &ipx_interfaces);
	spin_unlock_bh(&ipx_interfaces_lock);

	if (ipxcfg_auto_select_primary && !ipx_primary_net)
		ipx_primary_net = intrfc;
}
