static int string_to_crypto_compat_type(const char* crypto_compat_type)
{
	if (strcmp(crypto_compat_type, "2.0") == 0) {
		return CRYPTO_COMPAT_2_0;
	} else if (strcmp(crypto_compat_type, "2.1") == 0) {
		return CRYPTO_COMPAT_2_2;
	}
	return CRYPTO_COMPAT_2_2;
}
