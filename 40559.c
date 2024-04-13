static u32 gen_reqid(struct net *net)
{
	struct xfrm_policy_walk walk;
	u32 start;
	int rc;
	static u32 reqid = IPSEC_MANUAL_REQID_MAX;

	start = reqid;
	do {
		++reqid;
		if (reqid == 0)
			reqid = IPSEC_MANUAL_REQID_MAX+1;
		xfrm_policy_walk_init(&walk, XFRM_POLICY_TYPE_MAIN);
		rc = xfrm_policy_walk(net, &walk, check_reqid, (void*)&reqid);
		xfrm_policy_walk_done(&walk);
		if (rc != -EEXIST)
			return reqid;
	} while (reqid != start);
	return 0;
}
