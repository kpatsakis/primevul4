static int ecb3_encrypt(struct blkcipher_desc *desc,
		       struct scatterlist *dst, struct scatterlist *src,
		       unsigned int nbytes)
{
	return __ecb3_crypt(desc, dst, src, nbytes, true);
}
