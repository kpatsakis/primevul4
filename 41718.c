static int copy_from_user_migrate(struct xfrm_migrate *ma,
				  struct xfrm_kmaddress *k,
				  struct nlattr **attrs, int *num)
{
	struct nlattr *rt = attrs[XFRMA_MIGRATE];
	struct xfrm_user_migrate *um;
	int i, num_migrate;

	if (k != NULL) {
		struct xfrm_user_kmaddress *uk;

		uk = nla_data(attrs[XFRMA_KMADDRESS]);
		memcpy(&k->local, &uk->local, sizeof(k->local));
		memcpy(&k->remote, &uk->remote, sizeof(k->remote));
		k->family = uk->family;
		k->reserved = uk->reserved;
	}

	um = nla_data(rt);
	num_migrate = nla_len(rt) / sizeof(*um);

	if (num_migrate <= 0 || num_migrate > XFRM_MAX_DEPTH)
		return -EINVAL;

	for (i = 0; i < num_migrate; i++, um++, ma++) {
		memcpy(&ma->old_daddr, &um->old_daddr, sizeof(ma->old_daddr));
		memcpy(&ma->old_saddr, &um->old_saddr, sizeof(ma->old_saddr));
		memcpy(&ma->new_daddr, &um->new_daddr, sizeof(ma->new_daddr));
		memcpy(&ma->new_saddr, &um->new_saddr, sizeof(ma->new_saddr));

		ma->proto = um->proto;
		ma->mode = um->mode;
		ma->reqid = um->reqid;

		ma->old_family = um->old_family;
		ma->new_family = um->new_family;
	}

	*num = i;
	return 0;
}
