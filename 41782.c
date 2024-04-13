static int xfrm_send_policy_notify(struct xfrm_policy *xp, int dir, const struct km_event *c)
{

	switch (c->event) {
	case XFRM_MSG_NEWPOLICY:
	case XFRM_MSG_UPDPOLICY:
	case XFRM_MSG_DELPOLICY:
		return xfrm_notify_policy(xp, dir, c);
	case XFRM_MSG_FLUSHPOLICY:
		return xfrm_notify_policy_flush(c);
	case XFRM_MSG_POLEXPIRE:
		return xfrm_exp_policy_notify(xp, dir, c);
	default:
		printk(KERN_NOTICE "xfrm_user: Unknown Policy event %d\n",
		       c->event);
	}

	return 0;

}
