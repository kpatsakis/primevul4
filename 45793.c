static struct sock *x25_find_listener(struct x25_address *addr,
					struct sk_buff *skb)
{
	struct sock *s;
	struct sock *next_best;

	read_lock_bh(&x25_list_lock);
	next_best = NULL;

	sk_for_each(s, &x25_list)
		if ((!strcmp(addr->x25_addr,
			x25_sk(s)->source_addr.x25_addr) ||
				!strcmp(addr->x25_addr,
					null_x25_address.x25_addr)) &&
					s->sk_state == TCP_LISTEN) {
			/*
			 * Found a listening socket, now check the incoming
			 * call user data vs this sockets call user data
			 */
			if (x25_sk(s)->cudmatchlength > 0 &&
				skb->len >= x25_sk(s)->cudmatchlength) {
				if((memcmp(x25_sk(s)->calluserdata.cuddata,
					skb->data,
					x25_sk(s)->cudmatchlength)) == 0) {
					sock_hold(s);
					goto found;
				 }
			} else
				next_best = s;
		}
	if (next_best) {
		s = next_best;
		sock_hold(s);
		goto found;
	}
	s = NULL;
found:
	read_unlock_bh(&x25_list_lock);
	return s;
}
