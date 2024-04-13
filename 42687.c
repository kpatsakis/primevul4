SCTP_STATIC int sctp_setsockopt(struct sock *sk, int level, int optname,
				char __user *optval, int optlen)
{
	int retval = 0;

	SCTP_DEBUG_PRINTK("sctp_setsockopt(sk: %p... optname: %d)\n",
			  sk, optname);

	/* I can hardly begin to describe how wrong this is.  This is
	 * so broken as to be worse than useless.  The API draft
	 * REALLY is NOT helpful here...  I am not convinced that the
	 * semantics of setsockopt() with a level OTHER THAN SOL_SCTP
	 * are at all well-founded.
	 */
	if (level != SOL_SCTP) {
		struct sctp_af *af = sctp_sk(sk)->pf->af;
		retval = af->setsockopt(sk, level, optname, optval, optlen);
		goto out_nounlock;
	}

	sctp_lock_sock(sk);

	switch (optname) {
	case SCTP_SOCKOPT_BINDX_ADD:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_bindx(sk, (struct sockaddr __user *)optval,
					       optlen, SCTP_BINDX_ADD_ADDR);
		break;

	case SCTP_SOCKOPT_BINDX_REM:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_bindx(sk, (struct sockaddr __user *)optval,
					       optlen, SCTP_BINDX_REM_ADDR);
		break;

	case SCTP_SOCKOPT_CONNECTX:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_connectx(sk, (struct sockaddr __user *)optval,
					       optlen);
		break;

	case SCTP_DISABLE_FRAGMENTS:
		retval = sctp_setsockopt_disable_fragments(sk, optval, optlen);
		break;

	case SCTP_EVENTS:
		retval = sctp_setsockopt_events(sk, optval, optlen);
		break;

	case SCTP_AUTOCLOSE:
		retval = sctp_setsockopt_autoclose(sk, optval, optlen);
		break;

	case SCTP_PEER_ADDR_PARAMS:
		retval = sctp_setsockopt_peer_addr_params(sk, optval, optlen);
		break;

	case SCTP_DELAYED_ACK_TIME:
		retval = sctp_setsockopt_delayed_ack_time(sk, optval, optlen);
		break;

	case SCTP_INITMSG:
		retval = sctp_setsockopt_initmsg(sk, optval, optlen);
		break;
	case SCTP_DEFAULT_SEND_PARAM:
		retval = sctp_setsockopt_default_send_param(sk, optval,
							    optlen);
		break;
	case SCTP_PRIMARY_ADDR:
		retval = sctp_setsockopt_primary_addr(sk, optval, optlen);
		break;
	case SCTP_SET_PEER_PRIMARY_ADDR:
		retval = sctp_setsockopt_peer_primary_addr(sk, optval, optlen);
		break;
	case SCTP_NODELAY:
		retval = sctp_setsockopt_nodelay(sk, optval, optlen);
		break;
	case SCTP_RTOINFO:
		retval = sctp_setsockopt_rtoinfo(sk, optval, optlen);
		break;
	case SCTP_ASSOCINFO:
		retval = sctp_setsockopt_associnfo(sk, optval, optlen);
		break;
	case SCTP_I_WANT_MAPPED_V4_ADDR:
		retval = sctp_setsockopt_mappedv4(sk, optval, optlen);
		break;
	case SCTP_MAXSEG:
		retval = sctp_setsockopt_maxseg(sk, optval, optlen);
		break;
	case SCTP_ADAPTATION_LAYER:
		retval = sctp_setsockopt_adaptation_layer(sk, optval, optlen);
		break;
	case SCTP_CONTEXT:
		retval = sctp_setsockopt_context(sk, optval, optlen);
		break;

	default:
		retval = -ENOPROTOOPT;
		break;
	};

	sctp_release_sock(sk);

out_nounlock:
	return retval;
}
