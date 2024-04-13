static int pppol2tp_session_setsockopt(struct sock *sk,
				       struct l2tp_session *session,
				       int optname, int val)
{
	int err = 0;
	struct pppol2tp_session *ps = l2tp_session_priv(session);

	switch (optname) {
	case PPPOL2TP_SO_RECVSEQ:
		if ((val != 0) && (val != 1)) {
			err = -EINVAL;
			break;
		}
		session->recv_seq = val ? -1 : 0;
		l2tp_info(session, PPPOL2TP_MSG_CONTROL,
			  "%s: set recv_seq=%d\n",
			  session->name, session->recv_seq);
		break;

	case PPPOL2TP_SO_SENDSEQ:
		if ((val != 0) && (val != 1)) {
			err = -EINVAL;
			break;
		}
		session->send_seq = val ? -1 : 0;
		{
			struct sock *ssk      = ps->sock;
			struct pppox_sock *po = pppox_sk(ssk);
			po->chan.hdrlen = val ? PPPOL2TP_L2TP_HDR_SIZE_SEQ :
				PPPOL2TP_L2TP_HDR_SIZE_NOSEQ;
		}
		l2tp_info(session, PPPOL2TP_MSG_CONTROL,
			  "%s: set send_seq=%d\n",
			  session->name, session->send_seq);
		break;

	case PPPOL2TP_SO_LNSMODE:
		if ((val != 0) && (val != 1)) {
			err = -EINVAL;
			break;
		}
		session->lns_mode = val ? -1 : 0;
		l2tp_info(session, PPPOL2TP_MSG_CONTROL,
			  "%s: set lns_mode=%d\n",
			  session->name, session->lns_mode);
		break;

	case PPPOL2TP_SO_DEBUG:
		session->debug = val;
		l2tp_info(session, PPPOL2TP_MSG_CONTROL, "%s: set debug=%x\n",
			  session->name, session->debug);
		break;

	case PPPOL2TP_SO_REORDERTO:
		session->reorder_timeout = msecs_to_jiffies(val);
		l2tp_info(session, PPPOL2TP_MSG_CONTROL,
			  "%s: set reorder_timeout=%d\n",
			  session->name, session->reorder_timeout);
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	return err;
}
