static void __exit adf_unregister_ctl_device_driver(void)
{
	adf_chr_drv_destroy();
	adf_exit_aer();
	qat_crypto_unregister();
	qat_algs_exit();
	mutex_destroy(&adf_ctl_lock);
}
