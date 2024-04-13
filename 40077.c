struct sock *ax25_get_socket(ax25_address *my_addr, ax25_address *dest_addr,
	int type)
{
	struct sock *sk = NULL;
	ax25_cb *s;

	spin_lock(&ax25_list_lock);
	ax25_for_each(s, &ax25_list) {
		if (s->sk && !ax25cmp(&s->source_addr, my_addr) &&
		    !ax25cmp(&s->dest_addr, dest_addr) &&
		    s->sk->sk_type == type) {
			sk = s->sk;
			sock_hold(sk);
			break;
		}
	}

	spin_unlock(&ax25_list_lock);

	return sk;
}
