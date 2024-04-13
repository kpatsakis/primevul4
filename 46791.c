int udp_lib_setsockopt(struct sock *sk, int level, int optname,
		       char __user *optval, unsigned int optlen,
		       int (*push_pending_frames)(struct sock *))
{
	struct udp_sock *up = udp_sk(sk);
	int val, valbool;
	int err = 0;
	int is_udplite = IS_UDPLITE(sk);

	if (optlen < sizeof(int))
		return -EINVAL;

	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	valbool = val ? 1 : 0;

	switch (optname) {
	case UDP_CORK:
		if (val != 0) {
			up->corkflag = 1;
		} else {
			up->corkflag = 0;
			lock_sock(sk);
			push_pending_frames(sk);
			release_sock(sk);
		}
		break;

	case UDP_ENCAP:
		switch (val) {
		case 0:
		case UDP_ENCAP_ESPINUDP:
		case UDP_ENCAP_ESPINUDP_NON_IKE:
			up->encap_rcv = xfrm4_udp_encap_rcv;
			/* FALLTHROUGH */
		case UDP_ENCAP_L2TPINUDP:
			up->encap_type = val;
			udp_encap_enable();
			break;
		default:
			err = -ENOPROTOOPT;
			break;
		}
		break;

	case UDP_NO_CHECK6_TX:
		up->no_check6_tx = valbool;
		break;

	case UDP_NO_CHECK6_RX:
		up->no_check6_rx = valbool;
		break;

	/*
	 * 	UDP-Lite's partial checksum coverage (RFC 3828).
	 */
	/* The sender sets actual checksum coverage length via this option.
	 * The case coverage > packet length is handled by send module. */
	case UDPLITE_SEND_CSCOV:
		if (!is_udplite)         /* Disable the option on UDP sockets */
			return -ENOPROTOOPT;
		if (val != 0 && val < 8) /* Illegal coverage: use default (8) */
			val = 8;
		else if (val > USHRT_MAX)
			val = USHRT_MAX;
		up->pcslen = val;
		up->pcflag |= UDPLITE_SEND_CC;
		break;

	/* The receiver specifies a minimum checksum coverage value. To make
	 * sense, this should be set to at least 8 (as done below). If zero is
	 * used, this again means full checksum coverage.                     */
	case UDPLITE_RECV_CSCOV:
		if (!is_udplite)         /* Disable the option on UDP sockets */
			return -ENOPROTOOPT;
		if (val != 0 && val < 8) /* Avoid silly minimal values.       */
			val = 8;
		else if (val > USHRT_MAX)
			val = USHRT_MAX;
		up->pcrlen = val;
		up->pcflag |= UDPLITE_RECV_CC;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	return err;
}
