static int __init pcrypt_init(void)
{
	int err = -ENOMEM;

	pcrypt_kset = kset_create_and_add("pcrypt", NULL, kernel_kobj);
	if (!pcrypt_kset)
		goto err;

	err = pcrypt_init_padata(&pencrypt, "pencrypt");
	if (err)
		goto err_unreg_kset;

	err = pcrypt_init_padata(&pdecrypt, "pdecrypt");
	if (err)
		goto err_deinit_pencrypt;

	padata_start(pencrypt.pinst);
	padata_start(pdecrypt.pinst);

	return crypto_register_template(&pcrypt_tmpl);

err_deinit_pencrypt:
	pcrypt_fini_padata(&pencrypt);
err_unreg_kset:
	kset_unregister(pcrypt_kset);
err:
	return err;
}
