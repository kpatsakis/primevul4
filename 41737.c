static void verify_one_addr(struct nlattr **attrs, enum xfrm_attr_type_t type,
			   xfrm_address_t **addrp)
{
	struct nlattr *rt = attrs[type];

	if (rt && addrp)
		*addrp = nla_data(rt);
}
