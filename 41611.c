int sctp_asconf_mgmt(struct sctp_sock *sp, struct sctp_sockaddr_entry *addrw)
{
	struct sock *sk = sctp_opt2sk(sp);
	union sctp_addr *addr;
	struct sctp_af *af;

	/* It is safe to write port space in caller. */
	addr = &addrw->a;
	addr->v4.sin_port = htons(sp->ep->base.bind_addr.port);
	af = sctp_get_af_specific(addr->sa.sa_family);
	if (!af)
		return -EINVAL;
	if (sctp_verify_addr(sk, addr, af->sockaddr_len))
		return -EINVAL;

	if (addrw->state == SCTP_ADDR_NEW)
		return sctp_send_asconf_add_ip(sk, (struct sockaddr *)addr, 1);
	else
		return sctp_send_asconf_del_ip(sk, (struct sockaddr *)addr, 1);
}
