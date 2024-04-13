static int osap(struct tpm_buf *tb, struct osapsess *s,
		const unsigned char *key, uint16_t type, uint32_t handle)
{
	unsigned char enonce[TPM_NONCE_SIZE];
	unsigned char ononce[TPM_NONCE_SIZE];
	int ret;

	ret = tpm_get_random(TPM_ANY_NUM, ononce, TPM_NONCE_SIZE);
	if (ret != TPM_NONCE_SIZE)
		return ret;

	INIT_BUF(tb);
	store16(tb, TPM_TAG_RQU_COMMAND);
	store32(tb, TPM_OSAP_SIZE);
	store32(tb, TPM_ORD_OSAP);
	store16(tb, type);
	store32(tb, handle);
	storebytes(tb, ononce, TPM_NONCE_SIZE);

	ret = trusted_tpm_send(TPM_ANY_NUM, tb->data, MAX_BUF_SIZE);
	if (ret < 0)
		return ret;

	s->handle = LOAD32(tb->data, TPM_DATA_OFFSET);
	memcpy(s->enonce, &(tb->data[TPM_DATA_OFFSET + sizeof(uint32_t)]),
	       TPM_NONCE_SIZE);
	memcpy(enonce, &(tb->data[TPM_DATA_OFFSET + sizeof(uint32_t) +
				  TPM_NONCE_SIZE]), TPM_NONCE_SIZE);
	return TSS_rawhmac(s->secret, key, SHA1_DIGEST_SIZE, TPM_NONCE_SIZE,
			   enonce, TPM_NONCE_SIZE, ononce, 0, 0);
}
