static int sctp_getsockopt_hmac_ident(struct sock *sk, int len,
				    char __user *optval, int __user *optlen)
{
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	struct sctp_hmacalgo  __user *p = (void __user *)optval;
	struct sctp_hmac_algo_param *hmacs;
	__u16 data_len = 0;
	u32 num_idents;
	int i;

	if (!ep->auth_enable)
		return -EACCES;

	hmacs = ep->auth_hmacs_list;
	data_len = ntohs(hmacs->param_hdr.length) -
		   sizeof(struct sctp_paramhdr);

	if (len < sizeof(struct sctp_hmacalgo) + data_len)
		return -EINVAL;

	len = sizeof(struct sctp_hmacalgo) + data_len;
	num_idents = data_len / sizeof(u16);

	if (put_user(len, optlen))
		return -EFAULT;
	if (put_user(num_idents, &p->shmac_num_idents))
		return -EFAULT;
	for (i = 0; i < num_idents; i++) {
		__u16 hmacid = ntohs(hmacs->hmac_ids[i]);

		if (copy_to_user(&p->shmac_idents[i], &hmacid, sizeof(__u16)))
			return -EFAULT;
	}
	return 0;
}
