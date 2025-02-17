static int sctp_setsockopt_del_key(struct sock *sk,
				   char __user *optval,
				   unsigned int optlen)
{
	struct net *net = sock_net(sk);
	struct sctp_authkeyid val;
	struct sctp_association *asoc;

	if (!net->sctp.auth_enable)
		return -EACCES;

	if (optlen != sizeof(struct sctp_authkeyid))
		return -EINVAL;
	if (copy_from_user(&val, optval, optlen))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, val.scact_assoc_id);
	if (!asoc && val.scact_assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	return sctp_auth_del_key_id(sctp_sk(sk)->ep, asoc,
				    val.scact_keynumber);

}
