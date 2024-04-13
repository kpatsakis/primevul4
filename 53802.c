static int oiap(struct tpm_buf *tb, uint32_t *handle, unsigned char *nonce)
{
	int ret;

	INIT_BUF(tb);
	store16(tb, TPM_TAG_RQU_COMMAND);
	store32(tb, TPM_OIAP_SIZE);
	store32(tb, TPM_ORD_OIAP);
	ret = trusted_tpm_send(TPM_ANY_NUM, tb->data, MAX_BUF_SIZE);
	if (ret < 0)
		return ret;

	*handle = LOAD32(tb->data, TPM_DATA_OFFSET);
	memcpy(nonce, &tb->data[TPM_DATA_OFFSET + sizeof(uint32_t)],
	       TPM_NONCE_SIZE);
	return 0;
}
