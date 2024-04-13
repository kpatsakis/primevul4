static int ecb3_decrypt(struct blkcipher_desc *desc,
		       struct scatterlist *dst, struct scatterlist *src,
		       unsigned int nbytes)
{
	return __ecb3_crypt(desc, dst, src, nbytes, false);
}
