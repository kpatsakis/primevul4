static bool vmci_transport_allow_dgram(struct vsock_sock *vsock, u32 peer_cid)
{
	if (VMADDR_CID_HYPERVISOR == peer_cid)
		return true;

	if (vsock->cached_peer != peer_cid) {
		vsock->cached_peer = peer_cid;
		if (!vmci_transport_is_trusted(vsock, peer_cid) &&
		    (vmci_context_get_priv_flags(peer_cid) &
		     VMCI_PRIVILEGE_FLAG_RESTRICTED)) {
			vsock->cached_peer_allow_dgram = false;
		} else {
			vsock->cached_peer_allow_dgram = true;
		}
	}

	return vsock->cached_peer_allow_dgram;
}
