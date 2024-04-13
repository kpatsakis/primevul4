static void crypto_wait_for_test(struct crypto_larval *larval)
{
	int err;

	err = crypto_probing_notify(CRYPTO_MSG_ALG_REGISTER, larval->adult);
	if (err != NOTIFY_STOP) {
		if (WARN_ON(err != NOTIFY_DONE))
			goto out;
		crypto_alg_tested(larval->alg.cra_driver_name, 0);
	}

	err = wait_for_completion_interruptible(&larval->completion);
	WARN_ON(err);

out:
	crypto_larval_kill(&larval->alg);
}
