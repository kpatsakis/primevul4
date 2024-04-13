static int crypto_dump_report(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct crypto_alg *alg;
	struct crypto_dump_info info;
	int err;

	if (cb->args[0])
		goto out;

	cb->args[0] = 1;

	info.in_skb = cb->skb;
	info.out_skb = skb;
	info.nlmsg_seq = cb->nlh->nlmsg_seq;
	info.nlmsg_flags = NLM_F_MULTI;

	list_for_each_entry(alg, &crypto_alg_list, cra_list) {
		err = crypto_report_alg(alg, &info);
		if (err)
			goto out_err;
	}

out:
	return skb->len;
out_err:
	return err;
}
