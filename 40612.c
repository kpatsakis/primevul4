static int pfkey_send_notify(struct xfrm_state *x, const struct km_event *c)
{
	struct net *net = x ? xs_net(x) : c->net;
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);

	if (atomic_read(&net_pfkey->socks_nr) == 0)
		return 0;

	switch (c->event) {
	case XFRM_MSG_EXPIRE:
		return key_notify_sa_expire(x, c);
	case XFRM_MSG_DELSA:
	case XFRM_MSG_NEWSA:
	case XFRM_MSG_UPDSA:
		return key_notify_sa(x, c);
	case XFRM_MSG_FLUSHSA:
		return key_notify_sa_flush(c);
	case XFRM_MSG_NEWAE: /* not yet supported */
		break;
	default:
		pr_err("pfkey: Unknown SA event %d\n", c->event);
		break;
	}

	return 0;
}
