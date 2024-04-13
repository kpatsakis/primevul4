static struct trusted_key_options *trusted_options_alloc(void)
{
	struct trusted_key_options *options;
	int tpm2;

	tpm2 = tpm_is_tpm2(TPM_ANY_NUM);
	if (tpm2 < 0)
		return NULL;

	options = kzalloc(sizeof *options, GFP_KERNEL);
	if (options) {
		/* set any non-zero defaults */
		options->keytype = SRK_keytype;

		if (!tpm2)
			options->keyhandle = SRKHANDLE;
	}
	return options;
}
