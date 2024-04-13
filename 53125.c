static int __init init_evm(void)
{
	int error;

	evm_init_config();

	error = integrity_init_keyring(INTEGRITY_KEYRING_EVM);
	if (error)
		return error;

	error = evm_init_secfs();
	if (error < 0) {
		pr_info("Error registering secfs\n");
		return error;
	}

	return 0;
}
