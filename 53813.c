static int trusted_tpm_send(const u32 chip_num, unsigned char *cmd,
			    size_t buflen)
{
	int rc;

	dump_tpm_buf(cmd);
	rc = tpm_send(chip_num, cmd, buflen);
	dump_tpm_buf(cmd);
	if (rc > 0)
		/* Can't return positive return codes values to keyctl */
		rc = -EPERM;
	return rc;
}
