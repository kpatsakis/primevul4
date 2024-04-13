static int ceph_aes_encrypt2(const void *key, int key_len, void *dst,
			     size_t *dst_len,
			     const void *src1, size_t src1_len,
			     const void *src2, size_t src2_len)
{
	struct scatterlist sg_in[3], sg_out[1];
	struct crypto_blkcipher *tfm = ceph_crypto_alloc_cipher();
	struct blkcipher_desc desc = { .tfm = tfm, .flags = 0 };
	int ret;
	void *iv;
	int ivsize;
	size_t zero_padding = (0x10 - ((src1_len + src2_len) & 0x0f));
	char pad[16];

	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	memset(pad, zero_padding, zero_padding);

	*dst_len = src1_len + src2_len + zero_padding;

	crypto_blkcipher_setkey((void *)tfm, key, key_len);
	sg_init_table(sg_in, 3);
	sg_set_buf(&sg_in[0], src1, src1_len);
	sg_set_buf(&sg_in[1], src2, src2_len);
	sg_set_buf(&sg_in[2], pad, zero_padding);
	sg_init_table(sg_out, 1);
	sg_set_buf(sg_out, dst, *dst_len);
	iv = crypto_blkcipher_crt(tfm)->iv;
	ivsize = crypto_blkcipher_ivsize(tfm);

	memcpy(iv, aes_iv, ivsize);
	/*
	print_hex_dump(KERN_ERR, "enc  key: ", DUMP_PREFIX_NONE, 16, 1,
		       key, key_len, 1);
	print_hex_dump(KERN_ERR, "enc src1: ", DUMP_PREFIX_NONE, 16, 1,
			src1, src1_len, 1);
	print_hex_dump(KERN_ERR, "enc src2: ", DUMP_PREFIX_NONE, 16, 1,
			src2, src2_len, 1);
	print_hex_dump(KERN_ERR, "enc  pad: ", DUMP_PREFIX_NONE, 16, 1,
			pad, zero_padding, 1);
	*/
	ret = crypto_blkcipher_encrypt(&desc, sg_out, sg_in,
				     src1_len + src2_len + zero_padding);
	crypto_free_blkcipher(tfm);
	if (ret < 0)
		pr_err("ceph_aes_crypt2 failed %d\n", ret);
	/*
	print_hex_dump(KERN_ERR, "enc  out: ", DUMP_PREFIX_NONE, 16, 1,
		       dst, *dst_len, 1);
	*/
	return 0;
}
