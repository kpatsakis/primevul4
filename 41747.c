static int xfrm_add_sa_expire(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state *x;
	int err;
	struct xfrm_user_expire *ue = nlmsg_data(nlh);
	struct xfrm_usersa_info *p = &ue->state;
	struct xfrm_mark m;
	u32 mark = xfrm_mark_get(attrs, &m);

	x = xfrm_state_lookup(net, mark, &p->id.daddr, p->id.spi, p->id.proto, p->family);

	err = -ENOENT;
	if (x == NULL)
		return err;

	spin_lock_bh(&x->lock);
	err = -EINVAL;
	if (x->km.state != XFRM_STATE_VALID)
		goto out;
	km_state_expired(x, ue->hard, current->pid);

	if (ue->hard) {
		uid_t loginuid = audit_get_loginuid(current);
		u32 sessionid = audit_get_sessionid(current);
		u32 sid;

		security_task_getsecid(current, &sid);
		__xfrm_state_delete(x);
		xfrm_audit_state_delete(x, 1, loginuid, sessionid, sid);
	}
	err = 0;
out:
	spin_unlock_bh(&x->lock);
	xfrm_state_put(x);
	return err;
}
