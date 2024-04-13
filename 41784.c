static int xfrm_send_state_notify(struct xfrm_state *x, const struct km_event *c)
{

	switch (c->event) {
	case XFRM_MSG_EXPIRE:
		return xfrm_exp_state_notify(x, c);
	case XFRM_MSG_NEWAE:
		return xfrm_aevent_state_notify(x, c);
	case XFRM_MSG_DELSA:
	case XFRM_MSG_UPDSA:
	case XFRM_MSG_NEWSA:
		return xfrm_notify_sa(x, c);
	case XFRM_MSG_FLUSHSA:
		return xfrm_notify_sa_flush(c);
	default:
		printk(KERN_NOTICE "xfrm_user: Unknown SA event %d\n",
		       c->event);
		break;
	}

	return 0;

}
