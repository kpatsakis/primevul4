static void pfkey_dump_sa_done(struct pfkey_sock *pfk)
{
	xfrm_state_walk_done(&pfk->dump.u.state);
}
