static int build_aevent(struct sk_buff *skb, struct xfrm_state *x, const struct km_event *c)
{
	struct xfrm_aevent_id *id;
	struct nlmsghdr *nlh;
	int err;

	nlh = nlmsg_put(skb, c->pid, c->seq, XFRM_MSG_NEWAE, sizeof(*id), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	id = nlmsg_data(nlh);
	memcpy(&id->sa_id.daddr, &x->id.daddr,sizeof(x->id.daddr));
	id->sa_id.spi = x->id.spi;
	id->sa_id.family = x->props.family;
	id->sa_id.proto = x->id.proto;
	memcpy(&id->saddr, &x->props.saddr,sizeof(x->props.saddr));
	id->reqid = x->props.reqid;
	id->flags = c->data.aevent;

	if (x->replay_esn) {
		err = nla_put(skb, XFRMA_REPLAY_ESN_VAL,
			      xfrm_replay_state_esn_len(x->replay_esn),
			      x->replay_esn);
	} else {
		err = nla_put(skb, XFRMA_REPLAY_VAL, sizeof(x->replay),
			      &x->replay);
	}
	if (err)
		goto out_cancel;
	err = nla_put(skb, XFRMA_LTIME_VAL, sizeof(x->curlft), &x->curlft);
	if (err)
		goto out_cancel;

	if (id->flags & XFRM_AE_RTHR) {
		err = nla_put_u32(skb, XFRMA_REPLAY_THRESH, x->replay_maxdiff);
		if (err)
			goto out_cancel;
	}
	if (id->flags & XFRM_AE_ETHR) {
		err = nla_put_u32(skb, XFRMA_ETIMER_THRESH,
				  x->replay_maxage * 10 / HZ);
		if (err)
			goto out_cancel;
	}
	err = xfrm_mark_put(skb, &x->mark);
	if (err)
		goto out_cancel;

	return nlmsg_end(skb, nlh);

out_cancel:
	nlmsg_cancel(skb, nlh);
	return err;
}
