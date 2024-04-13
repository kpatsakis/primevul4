static int string_to_crypto_cipher_type(const char* crypto_cipher_type)
{
	if (strcmp(crypto_cipher_type, "none") == 0) {
		return CRYPTO_CIPHER_TYPE_NONE;
	} else if (strcmp(crypto_cipher_type, "aes256") == 0) {
		return CRYPTO_CIPHER_TYPE_AES256;
	} else if (strcmp(crypto_cipher_type, "aes192") == 0) {
		return CRYPTO_CIPHER_TYPE_AES192;
	} else if (strcmp(crypto_cipher_type, "aes128") == 0) {
		return CRYPTO_CIPHER_TYPE_AES128;
	} else if (strcmp(crypto_cipher_type, "3des") == 0) {
		return CRYPTO_CIPHER_TYPE_3DES;
	}
	return CRYPTO_CIPHER_TYPE_AES256;
}
