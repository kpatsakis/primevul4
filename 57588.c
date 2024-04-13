static int crypto_skcipher_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_blkcipher rblkcipher;
	struct skcipher_alg *skcipher = container_of(alg, struct skcipher_alg,
						     base);

	strncpy(rblkcipher.type, "skcipher", sizeof(rblkcipher.type));
	strncpy(rblkcipher.geniv, "<none>", sizeof(rblkcipher.geniv));

	rblkcipher.blocksize = alg->cra_blocksize;
	rblkcipher.min_keysize = skcipher->min_keysize;
	rblkcipher.max_keysize = skcipher->max_keysize;
	rblkcipher.ivsize = skcipher->ivsize;

	if (nla_put(skb, CRYPTOCFGA_REPORT_BLKCIPHER,
		    sizeof(struct crypto_report_blkcipher), &rblkcipher))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -EMSGSIZE;
}
