int ceph_decrypt2(struct ceph_crypto_key *secret,
			void *dst1, size_t *dst1_len,
			void *dst2, size_t *dst2_len,
			const void *src, size_t src_len)
{
	size_t t;

	switch (secret->type) {
	case CEPH_CRYPTO_NONE:
		if (*dst1_len + *dst2_len < src_len)
			return -ERANGE;
		t = min(*dst1_len, src_len);
		memcpy(dst1, src, t);
		*dst1_len = t;
		src += t;
		src_len -= t;
		if (src_len) {
			t = min(*dst2_len, src_len);
			memcpy(dst2, src, t);
			*dst2_len = t;
		}
		return 0;

	case CEPH_CRYPTO_AES:
		return ceph_aes_decrypt2(secret->key, secret->len,
					 dst1, dst1_len, dst2, dst2_len,
					 src, src_len);

	default:
		return -EINVAL;
	}
}
