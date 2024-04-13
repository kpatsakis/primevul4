static int pcrlock(const int pcrnum)
{
	unsigned char hash[SHA1_DIGEST_SIZE];
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	ret = tpm_get_random(TPM_ANY_NUM, hash, SHA1_DIGEST_SIZE);
	if (ret != SHA1_DIGEST_SIZE)
		return ret;
	return tpm_pcr_extend(TPM_ANY_NUM, pcrnum, hash) ? -EINVAL : 0;
}
