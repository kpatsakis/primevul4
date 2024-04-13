static int __init crypto_ctr_module_init(void)
{
	int err;

	err = crypto_register_template(&crypto_ctr_tmpl);
	if (err)
		goto out;

	err = crypto_register_template(&crypto_rfc3686_tmpl);
	if (err)
		goto out_drop_ctr;

out:
	return err;

out_drop_ctr:
	crypto_unregister_template(&crypto_ctr_tmpl);
	goto out;
}
