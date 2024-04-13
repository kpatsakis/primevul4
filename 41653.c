static int sctp_getsockopt_peer_addr_params(struct sock *sk, int len,
					    char __user *optval, int __user *optlen)
{
	struct sctp_paddrparams  params;
	struct sctp_transport   *trans = NULL;
	struct sctp_association *asoc = NULL;
	struct sctp_sock        *sp = sctp_sk(sk);

	if (len < sizeof(struct sctp_paddrparams))
		return -EINVAL;
	len = sizeof(struct sctp_paddrparams);
	if (copy_from_user(&params, optval, len))
		return -EFAULT;

	/* If an address other than INADDR_ANY is specified, and
	 * no transport is found, then the request is invalid.
	 */
	if (!sctp_is_any(sk, ( union sctp_addr *)&params.spp_address)) {
		trans = sctp_addr_id2transport(sk, &params.spp_address,
					       params.spp_assoc_id);
		if (!trans) {
			SCTP_DEBUG_PRINTK("Failed no transport\n");
			return -EINVAL;
		}
	}

	/* Get association, if assoc_id != 0 and the socket is a one
	 * to many style socket, and an association was not found, then
	 * the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params.spp_assoc_id);
	if (!asoc && params.spp_assoc_id && sctp_style(sk, UDP)) {
		SCTP_DEBUG_PRINTK("Failed no association\n");
		return -EINVAL;
	}

	if (trans) {
		/* Fetch transport values. */
		params.spp_hbinterval = jiffies_to_msecs(trans->hbinterval);
		params.spp_pathmtu    = trans->pathmtu;
		params.spp_pathmaxrxt = trans->pathmaxrxt;
		params.spp_sackdelay  = jiffies_to_msecs(trans->sackdelay);

		/*draft-11 doesn't say what to return in spp_flags*/
		params.spp_flags      = trans->param_flags;
	} else if (asoc) {
		/* Fetch association values. */
		params.spp_hbinterval = jiffies_to_msecs(asoc->hbinterval);
		params.spp_pathmtu    = asoc->pathmtu;
		params.spp_pathmaxrxt = asoc->pathmaxrxt;
		params.spp_sackdelay  = jiffies_to_msecs(asoc->sackdelay);

		/*draft-11 doesn't say what to return in spp_flags*/
		params.spp_flags      = asoc->param_flags;
	} else {
		/* Fetch socket values. */
		params.spp_hbinterval = sp->hbinterval;
		params.spp_pathmtu    = sp->pathmtu;
		params.spp_sackdelay  = sp->sackdelay;
		params.spp_pathmaxrxt = sp->pathmaxrxt;

		/*draft-11 doesn't say what to return in spp_flags*/
		params.spp_flags      = sp->param_flags;
	}

	if (copy_to_user(optval, &params, len))
		return -EFAULT;

	if (put_user(len, optlen))
		return -EFAULT;

	return 0;
}
