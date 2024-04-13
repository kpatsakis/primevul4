int ceph_encrypt(struct ceph_crypto_key *secret, void *dst, size_t *dst_len,
		 const void *src, size_t src_len)
{
	switch (secret->type) {
	case CEPH_CRYPTO_NONE:
		if (*dst_len < src_len)
			return -ERANGE;
		memcpy(dst, src, src_len);
		*dst_len = src_len;
		return 0;

	case CEPH_CRYPTO_AES:
		return ceph_aes_encrypt(secret->key, secret->len, dst,
					dst_len, src, src_len);

	default:
		return -EINVAL;
	}
}
