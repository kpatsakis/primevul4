static int ceph_aes_decrypt2(const void *key, int key_len,
			     void *dst1, size_t *dst1_len,
			     void *dst2, size_t *dst2_len,
			     const void *src, size_t src_len)
{
	struct scatterlist sg_in[1], sg_out[3];
	struct crypto_blkcipher *tfm = ceph_crypto_alloc_cipher();
	struct blkcipher_desc desc = { .tfm = tfm };
	char pad[16];
	void *iv;
	int ivsize;
	int ret;
	int last_byte;

	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	sg_init_table(sg_in, 1);
	sg_set_buf(sg_in, src, src_len);
	sg_init_table(sg_out, 3);
	sg_set_buf(&sg_out[0], dst1, *dst1_len);
	sg_set_buf(&sg_out[1], dst2, *dst2_len);
	sg_set_buf(&sg_out[2], pad, sizeof(pad));

	crypto_blkcipher_setkey((void *)tfm, key, key_len);
	iv = crypto_blkcipher_crt(tfm)->iv;
	ivsize = crypto_blkcipher_ivsize(tfm);

	memcpy(iv, aes_iv, ivsize);

	/*
	print_hex_dump(KERN_ERR, "dec  key: ", DUMP_PREFIX_NONE, 16, 1,
		       key, key_len, 1);
	print_hex_dump(KERN_ERR, "dec   in: ", DUMP_PREFIX_NONE, 16, 1,
		       src, src_len, 1);
	*/

	ret = crypto_blkcipher_decrypt(&desc, sg_out, sg_in, src_len);
	crypto_free_blkcipher(tfm);
	if (ret < 0) {
		pr_err("ceph_aes_decrypt failed %d\n", ret);
		return ret;
	}

	if (src_len <= *dst1_len)
		last_byte = ((char *)dst1)[src_len - 1];
	else if (src_len <= *dst1_len + *dst2_len)
		last_byte = ((char *)dst2)[src_len - *dst1_len - 1];
	else
		last_byte = pad[src_len - *dst1_len - *dst2_len - 1];
	if (last_byte <= 16 && src_len >= last_byte) {
		src_len -= last_byte;
	} else {
		pr_err("ceph_aes_decrypt got bad padding %d on src len %d\n",
		       last_byte, (int)src_len);
		return -EPERM;  /* bad padding */
	}

	if (src_len < *dst1_len) {
		*dst1_len = src_len;
		*dst2_len = 0;
	} else {
		*dst2_len = src_len - *dst1_len;
	}
	/*
	print_hex_dump(KERN_ERR, "dec  out1: ", DUMP_PREFIX_NONE, 16, 1,
		       dst1, *dst1_len, 1);
	print_hex_dump(KERN_ERR, "dec  out2: ", DUMP_PREFIX_NONE, 16, 1,
		       dst2, *dst2_len, 1);
	*/

	return 0;
}
