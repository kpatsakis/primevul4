void __init evm_load_x509(void)
{
	int rc;

	rc = integrity_load_x509(INTEGRITY_KEYRING_EVM, CONFIG_EVM_X509_PATH);
	if (!rc)
		evm_initialized |= EVM_INIT_X509;
}
