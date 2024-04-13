static int copy_to_user_policy_type(u8 type, struct sk_buff *skb)
{
	struct xfrm_userpolicy_type upt = {
		.type = type,
	};

	return nla_put(skb, XFRMA_POLICY_TYPE, sizeof(upt), &upt);
}
