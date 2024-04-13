static int sctp_setsockopt_auth_chunk(struct sock *sk,
				      char __user *optval,
				      unsigned int optlen)
{
	struct net *net = sock_net(sk);
	struct sctp_authchunk val;

	if (!net->sctp.auth_enable)
		return -EACCES;

	if (optlen != sizeof(struct sctp_authchunk))
		return -EINVAL;
	if (copy_from_user(&val, optval, optlen))
		return -EFAULT;

	switch (val.sauth_chunk) {
	case SCTP_CID_INIT:
	case SCTP_CID_INIT_ACK:
	case SCTP_CID_SHUTDOWN_COMPLETE:
	case SCTP_CID_AUTH:
		return -EINVAL;
	}

	/* add this chunk id to the endpoint */
	return sctp_auth_ep_add_chunkid(sctp_sk(sk)->ep, val.sauth_chunk);
}
