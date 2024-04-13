static int build_migrate(struct sk_buff *skb, const struct xfrm_migrate *m,
			 int num_migrate, const struct xfrm_kmaddress *k,
			 const struct xfrm_selector *sel, u8 dir, u8 type)
{
	const struct xfrm_migrate *mp;
	struct xfrm_userpolicy_id *pol_id;
	struct nlmsghdr *nlh;
	int i, err;

	nlh = nlmsg_put(skb, 0, 0, XFRM_MSG_MIGRATE, sizeof(*pol_id), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	pol_id = nlmsg_data(nlh);
	/* copy data from selector, dir, and type to the pol_id */
	memset(pol_id, 0, sizeof(*pol_id));
	memcpy(&pol_id->sel, sel, sizeof(pol_id->sel));
	pol_id->dir = dir;

	if (k != NULL) {
		err = copy_to_user_kmaddress(k, skb);
		if (err)
			goto out_cancel;
	}
	err = copy_to_user_policy_type(type, skb);
	if (err)
		goto out_cancel;
	for (i = 0, mp = m ; i < num_migrate; i++, mp++) {
		err = copy_to_user_migrate(mp, skb);
		if (err)
			goto out_cancel;
	}

	return nlmsg_end(skb, nlh);

out_cancel:
	nlmsg_cancel(skb, nlh);
	return err;
}
