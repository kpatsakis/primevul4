static int crypto_add_alg(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct nlattr **attrs)
{
	int exact = 0;
	const char *name;
	struct crypto_alg *alg;
	struct crypto_user_alg *p = nlmsg_data(nlh);
	struct nlattr *priority = attrs[CRYPTOCFGA_PRIORITY_VAL];

	if (strlen(p->cru_driver_name))
		exact = 1;

	if (priority && !exact)
		return -EINVAL;

	alg = crypto_alg_match(p, exact);
	if (alg)
		return -EEXIST;

	if (strlen(p->cru_driver_name))
		name = p->cru_driver_name;
	else
		name = p->cru_name;

	switch (p->cru_type & p->cru_mask & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_AEAD:
		alg = crypto_user_aead_alg(name, p->cru_type, p->cru_mask);
		break;
	case CRYPTO_ALG_TYPE_GIVCIPHER:
	case CRYPTO_ALG_TYPE_BLKCIPHER:
	case CRYPTO_ALG_TYPE_ABLKCIPHER:
		alg = crypto_user_skcipher_alg(name, p->cru_type, p->cru_mask);
		break;
	default:
		alg = crypto_alg_mod_lookup(name, p->cru_type, p->cru_mask);
	}

	if (IS_ERR(alg))
		return PTR_ERR(alg);

	down_write(&crypto_alg_sem);

	if (priority)
		alg->cra_priority = nla_get_u32(priority);

	up_write(&crypto_alg_sem);

	crypto_mod_put(alg);

	return 0;
}
