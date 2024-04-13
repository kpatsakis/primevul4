static int copy_from_user_tmpl(struct xfrm_policy *pol, struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_TMPL];

	if (!rt) {
		pol->xfrm_nr = 0;
	} else {
		struct xfrm_user_tmpl *utmpl = nla_data(rt);
		int nr = nla_len(rt) / sizeof(*utmpl);
		int err;

		err = validate_tmpl(nr, utmpl, pol->family);
		if (err)
			return err;

		copy_templates(pol, utmpl, nr);
	}
	return 0;
}
