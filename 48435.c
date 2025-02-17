static int cts_cbc_decrypt(struct crypto_cts_ctx *ctx,
			   struct blkcipher_desc *desc,
			   struct scatterlist *dst,
			   struct scatterlist *src,
			   unsigned int offset,
			   unsigned int nbytes)
{
	int bsize = crypto_blkcipher_blocksize(desc->tfm);
	u8 tmp[bsize];
	struct blkcipher_desc lcldesc;
	struct scatterlist sgsrc[1], sgdst[1];
	int lastn = nbytes - bsize;
	u8 iv[bsize];
	u8 s[bsize * 2], d[bsize * 2];
	int err;

	if (lastn < 0)
		return -EINVAL;

	sg_init_table(sgsrc, 1);
	sg_init_table(sgdst, 1);

	scatterwalk_map_and_copy(s, src, offset, nbytes, 0);

	lcldesc.tfm = ctx->child;
	lcldesc.info = iv;
	lcldesc.flags = desc->flags;

	/* 1. Decrypt Cn-1 (s) to create Dn (tmp)*/
	memset(iv, 0, sizeof(iv));
	sg_set_buf(&sgsrc[0], s, bsize);
	sg_set_buf(&sgdst[0], tmp, bsize);
	err = crypto_blkcipher_decrypt_iv(&lcldesc, sgdst, sgsrc, bsize);
	if (err)
		return err;
	/* 2. Pad Cn with zeros at the end to create C of length BB */
	memset(iv, 0, sizeof(iv));
	memcpy(iv, s + bsize, lastn);
	/* 3. Exclusive-or Dn (tmp) with C (iv) to create Xn (tmp) */
	crypto_xor(tmp, iv, bsize);
	/* 4. Select the first Ln bytes of Xn (tmp) to create Pn */
	memcpy(d + bsize, tmp, lastn);

	/* 5. Append the tail (BB - Ln) bytes of Xn (tmp) to Cn to create En */
	memcpy(s + bsize + lastn, tmp + lastn, bsize - lastn);
	/* 6. Decrypt En to create Pn-1 */
	memzero_explicit(iv, sizeof(iv));

	sg_set_buf(&sgsrc[0], s + bsize, bsize);
	sg_set_buf(&sgdst[0], d, bsize);
	err = crypto_blkcipher_decrypt_iv(&lcldesc, sgdst, sgsrc, bsize);

	/* XOR with previous block */
	crypto_xor(d, desc->info, bsize);

	scatterwalk_map_and_copy(d, dst, offset, nbytes, 1);

	memcpy(desc->info, s, bsize);
	return err;
}
