static int sctp_setsockopt_delayed_ack_time(struct sock *sk,
					    char __user *optval, int optlen)
{
	struct sctp_assoc_value  params;
	struct sctp_transport   *trans = NULL;
	struct sctp_association *asoc = NULL;
	struct sctp_sock        *sp = sctp_sk(sk);

	if (optlen != sizeof(struct sctp_assoc_value))
		return - EINVAL;

	if (copy_from_user(&params, optval, optlen))
		return -EFAULT;

	/* Validate value parameter. */
	if (params.assoc_value > 500)
		return -EINVAL;

	/* Get association, if assoc_id != 0 and the socket is a one
	 * to many style socket, and an association was not found, then
	 * the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (!asoc && params.assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	if (params.assoc_value) {
		if (asoc) {
			asoc->sackdelay =
				msecs_to_jiffies(params.assoc_value);
			asoc->param_flags =
				(asoc->param_flags & ~SPP_SACKDELAY) |
				SPP_SACKDELAY_ENABLE;
		} else {
			sp->sackdelay = params.assoc_value;
			sp->param_flags =
				(sp->param_flags & ~SPP_SACKDELAY) |
				SPP_SACKDELAY_ENABLE;
		}
	} else {
		if (asoc) {
			asoc->param_flags =
				(asoc->param_flags & ~SPP_SACKDELAY) |
				SPP_SACKDELAY_DISABLE;
		} else {
			sp->param_flags =
				(sp->param_flags & ~SPP_SACKDELAY) |
				SPP_SACKDELAY_DISABLE;
		}
	}

	/* If change is for association, also apply to each transport. */
	if (asoc) {
		struct list_head *pos;

		list_for_each(pos, &asoc->peer.transport_addr_list) {
			trans = list_entry(pos, struct sctp_transport,
					   transports);
			if (params.assoc_value) {
				trans->sackdelay =
					msecs_to_jiffies(params.assoc_value);
				trans->param_flags =
					(trans->param_flags & ~SPP_SACKDELAY) |
					SPP_SACKDELAY_ENABLE;
			} else {
				trans->param_flags =
					(trans->param_flags & ~SPP_SACKDELAY) |
					SPP_SACKDELAY_DISABLE;
			}
		}
	}

	return 0;
}
