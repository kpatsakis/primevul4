SCTP_STATIC int sctp_setsockopt_connectx_old(struct sock* sk,
				      struct sockaddr __user *addrs,
				      int addrs_size)
{
	return __sctp_setsockopt_connectx(sk, addrs, addrs_size, NULL);
}
