void ax25_send_to_raw(ax25_address *addr, struct sk_buff *skb, int proto)
{
	ax25_cb *s;
	struct sk_buff *copy;

	spin_lock(&ax25_list_lock);
	ax25_for_each(s, &ax25_list) {
		if (s->sk != NULL && ax25cmp(&s->source_addr, addr) == 0 &&
		    s->sk->sk_type == SOCK_RAW &&
		    s->sk->sk_protocol == proto &&
		    s->ax25_dev->dev == skb->dev &&
		    atomic_read(&s->sk->sk_rmem_alloc) <= s->sk->sk_rcvbuf) {
			if ((copy = skb_clone(skb, GFP_ATOMIC)) == NULL)
				continue;
			if (sock_queue_rcv_skb(s->sk, copy) != 0)
				kfree_skb(copy);
		}
	}
	spin_unlock(&ax25_list_lock);
}
