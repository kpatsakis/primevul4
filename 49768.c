static u8 *cbc_crypt_copy(const u8 *in, u8 *out, u32 *key,
			   u8 *iv, struct cword *cword, int count)
{
	/*
	 * Padlock prefetches extra data so we must provide mapped input buffers.
	 * Assume there are at least 16 bytes of stack already in use.
	 */
	u8 buf[AES_BLOCK_SIZE * (MAX_CBC_FETCH_BLOCKS - 1) + PADLOCK_ALIGNMENT - 1];
	u8 *tmp = PTR_ALIGN(&buf[0], PADLOCK_ALIGNMENT);

	memcpy(tmp, in, count * AES_BLOCK_SIZE);
	return rep_xcrypt_cbc(tmp, out, key, iv, cword, count);
}
