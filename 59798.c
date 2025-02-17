int ceph_encrypt2(struct ceph_crypto_key *secret, void *dst, size_t *dst_len,
		  const void *src1, size_t src1_len,
		  const void *src2, size_t src2_len)
{
	switch (secret->type) {
	case CEPH_CRYPTO_NONE:
		if (*dst_len < src1_len + src2_len)
			return -ERANGE;
		memcpy(dst, src1, src1_len);
		memcpy(dst + src1_len, src2, src2_len);
		*dst_len = src1_len + src2_len;
		return 0;

	case CEPH_CRYPTO_AES:
		return ceph_aes_encrypt2(secret->key, secret->len, dst, dst_len,
					 src1, src1_len, src2, src2_len);

	default:
		return -EINVAL;
	}
}
