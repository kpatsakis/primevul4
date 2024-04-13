static int xfrm_dump_sa_done(struct netlink_callback *cb)
{
	struct xfrm_state_walk *walk = (struct xfrm_state_walk *) &cb->args[1];
	xfrm_state_walk_done(walk);
	return 0;
}
