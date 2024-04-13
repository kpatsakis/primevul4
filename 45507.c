static struct ipx_interface *ipxitf_alloc(struct net_device *dev, __be32 netnum,
					  __be16 dlink_type,
					  struct datalink_proto *dlink,
					  unsigned char internal,
					  int ipx_offset)
{
	struct ipx_interface *intrfc = kmalloc(sizeof(*intrfc), GFP_ATOMIC);

	if (intrfc) {
		intrfc->if_dev		= dev;
		intrfc->if_netnum	= netnum;
		intrfc->if_dlink_type 	= dlink_type;
		intrfc->if_dlink 	= dlink;
		intrfc->if_internal 	= internal;
		intrfc->if_ipx_offset 	= ipx_offset;
		intrfc->if_sknum 	= IPX_MIN_EPHEMERAL_SOCKET;
		INIT_HLIST_HEAD(&intrfc->if_sklist);
		atomic_set(&intrfc->refcnt, 1);
		spin_lock_init(&intrfc->if_sklist_lock);
	}

	return intrfc;
}
