static void pfkey_dump_sp_done(struct pfkey_sock *pfk)
{
	xfrm_policy_walk_done(&pfk->dump.u.policy);
}
