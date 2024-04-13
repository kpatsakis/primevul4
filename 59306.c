static int sctp_setsockopt(struct sock *sk, int level, int optname,
			   char __user *optval, unsigned int optlen)
{
	int retval = 0;

	pr_debug("%s: sk:%p, optname:%d\n", __func__, sk, optname);

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

	lock_sock(sk);

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

	case SCTP_SOCKOPT_CONNECTX_OLD:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_connectx_old(sk,
					    (struct sockaddr __user *)optval,
					    optlen);
		break;

	case SCTP_SOCKOPT_CONNECTX:
		/* 'optlen' is the size of the addresses buffer. */
		retval = sctp_setsockopt_connectx(sk,
					    (struct sockaddr __user *)optval,
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

	case SCTP_DELAYED_SACK:
		retval = sctp_setsockopt_delayed_ack(sk, optval, optlen);
		break;
	case SCTP_PARTIAL_DELIVERY_POINT:
		retval = sctp_setsockopt_partial_delivery_point(sk, optval, optlen);
		break;

	case SCTP_INITMSG:
		retval = sctp_setsockopt_initmsg(sk, optval, optlen);
		break;
	case SCTP_DEFAULT_SEND_PARAM:
		retval = sctp_setsockopt_default_send_param(sk, optval,
							    optlen);
		break;
	case SCTP_DEFAULT_SNDINFO:
		retval = sctp_setsockopt_default_sndinfo(sk, optval, optlen);
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
	case SCTP_FRAGMENT_INTERLEAVE:
		retval = sctp_setsockopt_fragment_interleave(sk, optval, optlen);
		break;
	case SCTP_MAX_BURST:
		retval = sctp_setsockopt_maxburst(sk, optval, optlen);
		break;
	case SCTP_AUTH_CHUNK:
		retval = sctp_setsockopt_auth_chunk(sk, optval, optlen);
		break;
	case SCTP_HMAC_IDENT:
		retval = sctp_setsockopt_hmac_ident(sk, optval, optlen);
		break;
	case SCTP_AUTH_KEY:
		retval = sctp_setsockopt_auth_key(sk, optval, optlen);
		break;
	case SCTP_AUTH_ACTIVE_KEY:
		retval = sctp_setsockopt_active_key(sk, optval, optlen);
		break;
	case SCTP_AUTH_DELETE_KEY:
		retval = sctp_setsockopt_del_key(sk, optval, optlen);
		break;
	case SCTP_AUTO_ASCONF:
		retval = sctp_setsockopt_auto_asconf(sk, optval, optlen);
		break;
	case SCTP_PEER_ADDR_THLDS:
		retval = sctp_setsockopt_paddr_thresholds(sk, optval, optlen);
		break;
	case SCTP_RECVRCVINFO:
		retval = sctp_setsockopt_recvrcvinfo(sk, optval, optlen);
		break;
	case SCTP_RECVNXTINFO:
		retval = sctp_setsockopt_recvnxtinfo(sk, optval, optlen);
		break;
	case SCTP_PR_SUPPORTED:
		retval = sctp_setsockopt_pr_supported(sk, optval, optlen);
		break;
	case SCTP_DEFAULT_PRINFO:
		retval = sctp_setsockopt_default_prinfo(sk, optval, optlen);
		break;
	default:
		retval = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);

out_nounlock:
	return retval;
}
