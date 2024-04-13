static int mode_is_aes(enum cryp_algo_mode mode)
{
	return	CRYP_ALGO_AES_ECB == mode ||
		CRYP_ALGO_AES_CBC == mode ||
		CRYP_ALGO_AES_CTR == mode ||
		CRYP_ALGO_AES_XTS == mode;
}
