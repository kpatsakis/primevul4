static int sctp_ctl_sock_init(struct net *net)
{
	int err;
	sa_family_t family = PF_INET;

	if (sctp_get_pf_specific(PF_INET6))
		family = PF_INET6;

	err = inet_ctl_sock_create(&net->sctp.ctl_sock, family,
				   SOCK_SEQPACKET, IPPROTO_SCTP, net);

	/* If IPv6 socket could not be created, try the IPv4 socket */
	if (err < 0 && family == PF_INET6)
		err = inet_ctl_sock_create(&net->sctp.ctl_sock, AF_INET,
					   SOCK_SEQPACKET, IPPROTO_SCTP,
					   net);

	if (err < 0) {
		pr_err("Failed to create the SCTP control socket\n");
		return err;
	}
	return 0;
}
