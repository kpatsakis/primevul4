static int rfcomm_sock_getname(struct socket *sock, struct sockaddr *addr, int *len, int peer)
{
	struct sockaddr_rc *sa = (struct sockaddr_rc *) addr;
	struct sock *sk = sock->sk;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (peer && sk->sk_state != BT_CONNECTED &&
	    sk->sk_state != BT_CONNECT && sk->sk_state != BT_CONNECT2)
		return -ENOTCONN;

	memset(sa, 0, sizeof(*sa));
	sa->rc_family  = AF_BLUETOOTH;
	sa->rc_channel = rfcomm_pi(sk)->channel;
	if (peer)
		bacpy(&sa->rc_bdaddr, &rfcomm_pi(sk)->dst);
	else
		bacpy(&sa->rc_bdaddr, &rfcomm_pi(sk)->src);

	*len = sizeof(struct sockaddr_rc);
	return 0;
}
