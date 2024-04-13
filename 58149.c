static int __init crypto_ccm_module_init(void)
{
	int err;

	err = crypto_register_template(&crypto_cbcmac_tmpl);
	if (err)
		goto out;

	err = crypto_register_template(&crypto_ccm_base_tmpl);
	if (err)
		goto out_undo_cbcmac;

	err = crypto_register_template(&crypto_ccm_tmpl);
	if (err)
		goto out_undo_base;

	err = crypto_register_template(&crypto_rfc4309_tmpl);
	if (err)
		goto out_undo_ccm;

out:
	return err;

out_undo_ccm:
	crypto_unregister_template(&crypto_ccm_tmpl);
out_undo_base:
	crypto_unregister_template(&crypto_ccm_base_tmpl);
out_undo_cbcmac:
	crypto_register_template(&crypto_cbcmac_tmpl);
	goto out;
}
