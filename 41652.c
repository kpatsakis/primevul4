static int sctp_getsockopt_peer_addr_info(struct sock *sk, int len,
					  char __user *optval,
					  int __user *optlen)
{
	struct sctp_paddrinfo pinfo;
	struct sctp_transport *transport;
	int retval = 0;

	if (len < sizeof(pinfo)) {
		retval = -EINVAL;
		goto out;
	}

	len = sizeof(pinfo);
	if (copy_from_user(&pinfo, optval, len)) {
		retval = -EFAULT;
		goto out;
	}

	transport = sctp_addr_id2transport(sk, &pinfo.spinfo_address,
					   pinfo.spinfo_assoc_id);
	if (!transport)
		return -EINVAL;

	pinfo.spinfo_assoc_id = sctp_assoc2id(transport->asoc);
	pinfo.spinfo_state = transport->state;
	pinfo.spinfo_cwnd = transport->cwnd;
	pinfo.spinfo_srtt = transport->srtt;
	pinfo.spinfo_rto = jiffies_to_msecs(transport->rto);
	pinfo.spinfo_mtu = transport->pathmtu;

	if (pinfo.spinfo_state == SCTP_UNKNOWN)
		pinfo.spinfo_state = SCTP_ACTIVE;

	if (put_user(len, optlen)) {
		retval = -EFAULT;
		goto out;
	}

	if (copy_to_user(optval, &pinfo, len)) {
		retval = -EFAULT;
		goto out;
	}

out:
	return retval;
}
