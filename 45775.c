vmci_transport_packet_init(struct vmci_transport_packet *pkt,
			   struct sockaddr_vm *src,
			   struct sockaddr_vm *dst,
			   u8 type,
			   u64 size,
			   u64 mode,
			   struct vmci_transport_waiting_info *wait,
			   u16 proto,
			   struct vmci_handle handle)
{
	/* We register the stream control handler as an any cid handle so we
	 * must always send from a source address of VMADDR_CID_ANY
	 */
	pkt->dg.src = vmci_make_handle(VMADDR_CID_ANY,
				       VMCI_TRANSPORT_PACKET_RID);
	pkt->dg.dst = vmci_make_handle(dst->svm_cid,
				       vmci_transport_peer_rid(dst->svm_cid));
	pkt->dg.payload_size = sizeof(*pkt) - sizeof(pkt->dg);
	pkt->version = VMCI_TRANSPORT_PACKET_VERSION;
	pkt->type = type;
	pkt->src_port = src->svm_port;
	pkt->dst_port = dst->svm_port;
	memset(&pkt->proto, 0, sizeof(pkt->proto));
	memset(&pkt->_reserved2, 0, sizeof(pkt->_reserved2));

	switch (pkt->type) {
	case VMCI_TRANSPORT_PACKET_TYPE_INVALID:
		pkt->u.size = 0;
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_REQUEST:
	case VMCI_TRANSPORT_PACKET_TYPE_NEGOTIATE:
		pkt->u.size = size;
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_OFFER:
	case VMCI_TRANSPORT_PACKET_TYPE_ATTACH:
		pkt->u.handle = handle;
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_WROTE:
	case VMCI_TRANSPORT_PACKET_TYPE_READ:
	case VMCI_TRANSPORT_PACKET_TYPE_RST:
		pkt->u.size = 0;
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_SHUTDOWN:
		pkt->u.mode = mode;
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_WAITING_READ:
	case VMCI_TRANSPORT_PACKET_TYPE_WAITING_WRITE:
		memcpy(&pkt->u.wait, wait, sizeof(pkt->u.wait));
		break;

	case VMCI_TRANSPORT_PACKET_TYPE_REQUEST2:
	case VMCI_TRANSPORT_PACKET_TYPE_NEGOTIATE2:
		pkt->u.size = size;
		pkt->proto = proto;
		break;
	}
}
