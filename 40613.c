static int pfkey_send_policy_notify(struct xfrm_policy *xp, int dir, const struct km_event *c)
{
	if (xp && xp->type != XFRM_POLICY_TYPE_MAIN)
		return 0;

	switch (c->event) {
	case XFRM_MSG_POLEXPIRE:
		return key_notify_policy_expire(xp, c);
	case XFRM_MSG_DELPOLICY:
	case XFRM_MSG_NEWPOLICY:
	case XFRM_MSG_UPDPOLICY:
		return key_notify_policy(xp, dir, c);
	case XFRM_MSG_FLUSHPOLICY:
		if (c->data.type != XFRM_POLICY_TYPE_MAIN)
			break;
		return key_notify_policy_flush(c);
	default:
		pr_err("pfkey: Unknown policy event %d\n", c->event);
		break;
	}

	return 0;
}
