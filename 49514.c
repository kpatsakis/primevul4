static struct crypto_alg *crypto_larval_wait(struct crypto_alg *alg)
{
	struct crypto_larval *larval = (void *)alg;
	long timeout;

	timeout = wait_for_completion_interruptible_timeout(
		&larval->completion, 60 * HZ);

	alg = larval->adult;
	if (timeout < 0)
		alg = ERR_PTR(-EINTR);
	else if (!timeout)
		alg = ERR_PTR(-ETIMEDOUT);
	else if (!alg)
		alg = ERR_PTR(-ENOENT);
	else if (crypto_is_test_larval(larval) &&
		 !(alg->cra_flags & CRYPTO_ALG_TESTED))
		alg = ERR_PTR(-EAGAIN);
	else if (!crypto_mod_get(alg))
		alg = ERR_PTR(-EAGAIN);
	crypto_mod_put(&larval->alg);

	return alg;
}
