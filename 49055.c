static int ccm_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	if ((authsize & 1) || authsize < 4)
		return -EINVAL;
	return 0;
}
