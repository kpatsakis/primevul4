static int packet_getsockopt(struct socket *sock, int level, int optname,
			     char __user *optval, int __user *optlen)
{
	int len;
	int val, lv = sizeof(val);
	struct sock *sk = sock->sk;
	struct packet_sock *po = pkt_sk(sk);
	void *data = &val;
	union tpacket_stats_u st;

	if (level != SOL_PACKET)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case PACKET_STATISTICS:
		spin_lock_bh(&sk->sk_receive_queue.lock);
		memcpy(&st, &po->stats, sizeof(st));
		memset(&po->stats, 0, sizeof(po->stats));
		spin_unlock_bh(&sk->sk_receive_queue.lock);

		if (po->tp_version == TPACKET_V3) {
			lv = sizeof(struct tpacket_stats_v3);
			st.stats3.tp_packets += st.stats3.tp_drops;
			data = &st.stats3;
		} else {
			lv = sizeof(struct tpacket_stats);
			st.stats1.tp_packets += st.stats1.tp_drops;
			data = &st.stats1;
		}

		break;
	case PACKET_AUXDATA:
		val = po->auxdata;
		break;
	case PACKET_ORIGDEV:
		val = po->origdev;
		break;
	case PACKET_VNET_HDR:
		val = po->has_vnet_hdr;
		break;
	case PACKET_VERSION:
		val = po->tp_version;
		break;
	case PACKET_HDRLEN:
		if (len > sizeof(int))
			len = sizeof(int);
		if (copy_from_user(&val, optval, len))
			return -EFAULT;
		switch (val) {
		case TPACKET_V1:
			val = sizeof(struct tpacket_hdr);
			break;
		case TPACKET_V2:
			val = sizeof(struct tpacket2_hdr);
			break;
		case TPACKET_V3:
			val = sizeof(struct tpacket3_hdr);
			break;
		default:
			return -EINVAL;
		}
		break;
	case PACKET_RESERVE:
		val = po->tp_reserve;
		break;
	case PACKET_LOSS:
		val = po->tp_loss;
		break;
	case PACKET_TIMESTAMP:
		val = po->tp_tstamp;
		break;
	case PACKET_FANOUT:
		val = (po->fanout ?
		       ((u32)po->fanout->id |
			((u32)po->fanout->type << 16) |
			((u32)po->fanout->flags << 24)) :
		       0);
		break;
	case PACKET_TX_HAS_OFF:
		val = po->tp_tx_has_off;
		break;
	default:
		return -ENOPROTOOPT;
	}

	if (len > lv)
		len = lv;
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, data, len))
		return -EFAULT;
	return 0;
}
