static int copy_from_user_policy_type(u8 *tp, struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_POLICY_TYPE];
	struct xfrm_userpolicy_type *upt;
	u8 type = XFRM_POLICY_TYPE_MAIN;
	int err;

	if (rt) {
		upt = nla_data(rt);
		type = upt->type;
	}

	err = verify_policy_type(type);
	if (err)
		return err;

	*tp = type;
	return 0;
}
