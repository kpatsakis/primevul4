static struct sock *ncp_connection_hack(struct ipx_interface *intrfc,
					struct ipxhdr *ipx)
{
	/* The packet's target is a NCP connection handler. We want to hand it
	 * to the correct socket directly within the kernel, so that the
	 * mars_nwe packet distribution process does not have to do it. Here we
	 * only care about NCP and BURST packets.
	 *
	 * You might call this a hack, but believe me, you do not want a
	 * complete NCP layer in the kernel, and this is VERY fast as well. */
	struct sock *sk = NULL;
	int connection = 0;
	u8 *ncphdr = (u8 *)(ipx + 1);

	if (*ncphdr == 0x22 && *(ncphdr + 1) == 0x22) /* NCP request */
		connection = (((int) *(ncphdr + 5)) << 8) | (int) *(ncphdr + 3);
	else if (*ncphdr == 0x77 && *(ncphdr + 1) == 0x77) /* BURST packet */
		connection = (((int) *(ncphdr + 9)) << 8) | (int) *(ncphdr + 8);

	if (connection) {
		/* Now we have to look for a special NCP connection handling
		 * socket. Only these sockets have ipx_ncp_conn != 0, set by
		 * SIOCIPXNCPCONN. */
		spin_lock_bh(&intrfc->if_sklist_lock);
		sk_for_each(sk, &intrfc->if_sklist)
			if (ipx_sk(sk)->ipx_ncp_conn == connection) {
				sock_hold(sk);
				goto found;
			}
		sk = NULL;
	found:
		spin_unlock_bh(&intrfc->if_sklist_lock);
	}
	return sk;
}
