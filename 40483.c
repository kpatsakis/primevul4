static int crypto_del_alg(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct nlattr **attrs)
{
	struct crypto_alg *alg;
	struct crypto_user_alg *p = nlmsg_data(nlh);

	alg = crypto_alg_match(p, 1);
	if (!alg)
		return -ENOENT;

	/* We can not unregister core algorithms such as aes-generic.
	 * We would loose the reference in the crypto_alg_list to this algorithm
	 * if we try to unregister. Unregistering such an algorithm without
	 * removing the module is not possible, so we restrict to crypto
	 * instances that are build from templates. */
	if (!(alg->cra_flags & CRYPTO_ALG_INSTANCE))
		return -EINVAL;

	if (atomic_read(&alg->cra_refcnt) != 1)
		return -EBUSY;

	return crypto_unregister_instance(alg);
}
