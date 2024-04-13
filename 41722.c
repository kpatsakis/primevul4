static void copy_from_user_state(struct xfrm_state *x, struct xfrm_usersa_info *p)
{
	memcpy(&x->id, &p->id, sizeof(x->id));
	memcpy(&x->sel, &p->sel, sizeof(x->sel));
	memcpy(&x->lft, &p->lft, sizeof(x->lft));
	x->props.mode = p->mode;
	x->props.replay_window = p->replay_window;
	x->props.reqid = p->reqid;
	x->props.family = p->family;
	memcpy(&x->props.saddr, &p->saddr, sizeof(x->props.saddr));
	x->props.flags = p->flags;

	if (!x->sel.family && !(p->flags & XFRM_STATE_AF_UNSPEC))
		x->sel.family = p->family;
}
