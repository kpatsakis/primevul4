static inline u8 *cbc_crypt(const u8 *in, u8 *out, u32 *key,
			    u8 *iv, struct cword *cword, int count)
{
	/* Padlock in CBC mode fetches at least cbc_fetch_bytes of data. */
	if (unlikely(((unsigned long)in & ~PAGE_MASK) + cbc_fetch_bytes > PAGE_SIZE))
		return cbc_crypt_copy(in, out, key, iv, cword, count);

	return rep_xcrypt_cbc(in, out, key, iv, cword, count);
}
