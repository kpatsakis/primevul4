brcmf_notify_tdls_peer_event(struct brcmf_if *ifp,
			     const struct brcmf_event_msg *e, void *data)
{
	switch (e->reason) {
	case BRCMF_E_REASON_TDLS_PEER_DISCOVERED:
		brcmf_dbg(TRACE, "TDLS Peer Discovered\n");
		break;
	case BRCMF_E_REASON_TDLS_PEER_CONNECTED:
		brcmf_dbg(TRACE, "TDLS Peer Connected\n");
		brcmf_proto_add_tdls_peer(ifp->drvr, ifp->ifidx, (u8 *)e->addr);
		break;
	case BRCMF_E_REASON_TDLS_PEER_DISCONNECTED:
		brcmf_dbg(TRACE, "TDLS Peer Disconnected\n");
		brcmf_proto_delete_peer(ifp->drvr, ifp->ifidx, (u8 *)e->addr);
		break;
	}

	return 0;
}
