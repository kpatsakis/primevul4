static int sctp_setsockopt_maxseg(struct sock *sk, char __user *optval, int optlen)
{
	struct sctp_association *asoc;
	struct list_head *pos;
	struct sctp_sock *sp = sctp_sk(sk);
	int val;

	if (optlen < sizeof(int))
		return -EINVAL;
	if (get_user(val, (int __user *)optval))
		return -EFAULT;
	if ((val != 0) && ((val < 8) || (val > SCTP_MAX_CHUNK_LEN)))
		return -EINVAL;
	sp->user_frag = val;

	/* Update the frag_point of the existing associations. */
	list_for_each(pos, &(sp->ep->asocs)) {
		asoc = list_entry(pos, struct sctp_association, asocs);
		asoc->frag_point = sctp_frag_point(sp, asoc->pathmtu);
	}

	return 0;
}
