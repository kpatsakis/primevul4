static int __init adf_register_ctl_device_driver(void)
{
	mutex_init(&adf_ctl_lock);

	if (qat_algs_init())
		goto err_algs_init;

	if (adf_chr_drv_create())
		goto err_chr_dev;

	if (adf_init_aer())
		goto err_aer;

	if (qat_crypto_register())
		goto err_crypto_register;

	return 0;

err_crypto_register:
	adf_exit_aer();
err_aer:
	adf_chr_drv_destroy();
err_chr_dev:
	qat_algs_exit();
err_algs_init:
	mutex_destroy(&adf_ctl_lock);
	return -EFAULT;
}
