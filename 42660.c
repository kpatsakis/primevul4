SCTP_STATIC int sctp_getsockopt(struct sock *sk, int level, int optname,
				char __user *optval, int __user *optlen)
{
	int retval = 0;
	int len;

	SCTP_DEBUG_PRINTK("sctp_getsockopt(sk: %p... optname: %d)\n",
			  sk, optname);

	/* I can hardly begin to describe how wrong this is.  This is
	 * so broken as to be worse than useless.  The API draft
	 * REALLY is NOT helpful here...  I am not convinced that the
	 * semantics of getsockopt() with a level OTHER THAN SOL_SCTP
	 * are at all well-founded.
	 */
	if (level != SOL_SCTP) {
		struct sctp_af *af = sctp_sk(sk)->pf->af;

		retval = af->getsockopt(sk, level, optname, optval, optlen);
		return retval;
	}

	if (get_user(len, optlen))
		return -EFAULT;

	sctp_lock_sock(sk);

	switch (optname) {
	case SCTP_STATUS:
		retval = sctp_getsockopt_sctp_status(sk, len, optval, optlen);
		break;
	case SCTP_DISABLE_FRAGMENTS:
		retval = sctp_getsockopt_disable_fragments(sk, len, optval,
							   optlen);
		break;
	case SCTP_EVENTS:
		retval = sctp_getsockopt_events(sk, len, optval, optlen);
		break;
	case SCTP_AUTOCLOSE:
		retval = sctp_getsockopt_autoclose(sk, len, optval, optlen);
		break;
	case SCTP_SOCKOPT_PEELOFF:
		retval = sctp_getsockopt_peeloff(sk, len, optval, optlen);
		break;
	case SCTP_PEER_ADDR_PARAMS:
		retval = sctp_getsockopt_peer_addr_params(sk, len, optval,
							  optlen);
		break;
	case SCTP_DELAYED_ACK_TIME:
		retval = sctp_getsockopt_delayed_ack_time(sk, len, optval,
							  optlen);
		break;
	case SCTP_INITMSG:
		retval = sctp_getsockopt_initmsg(sk, len, optval, optlen);
		break;
	case SCTP_GET_PEER_ADDRS_NUM_OLD:
		retval = sctp_getsockopt_peer_addrs_num_old(sk, len, optval,
							    optlen);
		break;
	case SCTP_GET_LOCAL_ADDRS_NUM_OLD:
		retval = sctp_getsockopt_local_addrs_num_old(sk, len, optval,
							     optlen);
		break;
	case SCTP_GET_PEER_ADDRS_OLD:
		retval = sctp_getsockopt_peer_addrs_old(sk, len, optval,
							optlen);
		break;
	case SCTP_GET_LOCAL_ADDRS_OLD:
		retval = sctp_getsockopt_local_addrs_old(sk, len, optval,
							 optlen);
		break;
	case SCTP_GET_PEER_ADDRS:
		retval = sctp_getsockopt_peer_addrs(sk, len, optval,
						    optlen);
		break;
	case SCTP_GET_LOCAL_ADDRS:
		retval = sctp_getsockopt_local_addrs(sk, len, optval,
						     optlen);
		break;
	case SCTP_DEFAULT_SEND_PARAM:
		retval = sctp_getsockopt_default_send_param(sk, len,
							    optval, optlen);
		break;
	case SCTP_PRIMARY_ADDR:
		retval = sctp_getsockopt_primary_addr(sk, len, optval, optlen);
		break;
	case SCTP_NODELAY:
		retval = sctp_getsockopt_nodelay(sk, len, optval, optlen);
		break;
	case SCTP_RTOINFO:
		retval = sctp_getsockopt_rtoinfo(sk, len, optval, optlen);
		break;
	case SCTP_ASSOCINFO:
		retval = sctp_getsockopt_associnfo(sk, len, optval, optlen);
		break;
	case SCTP_I_WANT_MAPPED_V4_ADDR:
		retval = sctp_getsockopt_mappedv4(sk, len, optval, optlen);
		break;
	case SCTP_MAXSEG:
		retval = sctp_getsockopt_maxseg(sk, len, optval, optlen);
		break;
	case SCTP_GET_PEER_ADDR_INFO:
		retval = sctp_getsockopt_peer_addr_info(sk, len, optval,
							optlen);
		break;
	case SCTP_ADAPTATION_LAYER:
		retval = sctp_getsockopt_adaptation_layer(sk, len, optval,
							optlen);
		break;
	case SCTP_CONTEXT:
		retval = sctp_getsockopt_context(sk, len, optval, optlen);
		break;
	default:
		retval = -ENOPROTOOPT;
		break;
	};

	sctp_release_sock(sk);
	return retval;
}
