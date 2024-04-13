static int __init crypto_gcm_module_init(void)
{
	int err;

	gcm_zeroes = kzalloc(16, GFP_KERNEL);
	if (!gcm_zeroes)
		return -ENOMEM;

	err = crypto_register_template(&crypto_gcm_base_tmpl);
	if (err)
		goto out;

	err = crypto_register_template(&crypto_gcm_tmpl);
	if (err)
		goto out_undo_base;

	err = crypto_register_template(&crypto_rfc4106_tmpl);
	if (err)
		goto out_undo_gcm;

	err = crypto_register_template(&crypto_rfc4543_tmpl);
	if (err)
		goto out_undo_rfc4106;

	return 0;

out_undo_rfc4106:
	crypto_unregister_template(&crypto_rfc4106_tmpl);
out_undo_gcm:
	crypto_unregister_template(&crypto_gcm_tmpl);
out_undo_base:
	crypto_unregister_template(&crypto_gcm_base_tmpl);
out:
	kfree(gcm_zeroes);
	return err;
}
