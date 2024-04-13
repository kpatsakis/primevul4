static int __packet_rcv_has_room(struct packet_sock *po, struct sk_buff *skb)
{
	struct sock *sk = &po->sk;
	int ret = ROOM_NONE;

	if (po->prot_hook.func != tpacket_rcv) {
		int avail = sk->sk_rcvbuf - atomic_read(&sk->sk_rmem_alloc)
					  - (skb ? skb->truesize : 0);
		if (avail > (sk->sk_rcvbuf >> ROOM_POW_OFF))
			return ROOM_NORMAL;
		else if (avail > 0)
			return ROOM_LOW;
		else
			return ROOM_NONE;
	}

	if (po->tp_version == TPACKET_V3) {
		if (__tpacket_v3_has_room(po, ROOM_POW_OFF))
			ret = ROOM_NORMAL;
		else if (__tpacket_v3_has_room(po, 0))
			ret = ROOM_LOW;
	} else {
		if (__tpacket_has_room(po, ROOM_POW_OFF))
			ret = ROOM_NORMAL;
		else if (__tpacket_has_room(po, 0))
			ret = ROOM_LOW;
	}

	return ret;
}
