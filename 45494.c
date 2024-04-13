static __be16 ipx_first_free_socketnum(struct ipx_interface *intrfc)
{
	unsigned short socketNum = intrfc->if_sknum;

	spin_lock_bh(&intrfc->if_sklist_lock);

	if (socketNum < IPX_MIN_EPHEMERAL_SOCKET)
		socketNum = IPX_MIN_EPHEMERAL_SOCKET;

	while (__ipxitf_find_socket(intrfc, htons(socketNum)))
		if (socketNum > IPX_MAX_EPHEMERAL_SOCKET)
			socketNum = IPX_MIN_EPHEMERAL_SOCKET;
		else
			socketNum++;

	spin_unlock_bh(&intrfc->if_sklist_lock);
	intrfc->if_sknum = socketNum;

	return htons(socketNum);
}
