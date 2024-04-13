static int derived_key_decrypt(struct encrypted_key_payload *epayload,
			       const u8 *derived_key,
			       unsigned int derived_keylen)
{
	struct scatterlist sg_in[1];
	struct scatterlist sg_out[2];
	struct blkcipher_desc desc;
	unsigned int encrypted_datalen;
	char pad[16];
	int ret;

	encrypted_datalen = roundup(epayload->decrypted_datalen, blksize);
	ret = init_blkcipher_desc(&desc, derived_key, derived_keylen,
				  epayload->iv, ivsize);
	if (ret < 0)
		goto out;
	dump_encrypted_data(epayload, encrypted_datalen);

	memset(pad, 0, sizeof pad);
	sg_init_table(sg_in, 1);
	sg_init_table(sg_out, 2);
	sg_set_buf(sg_in, epayload->encrypted_data, encrypted_datalen);
	sg_set_buf(&sg_out[0], epayload->decrypted_data,
		   epayload->decrypted_datalen);
	sg_set_buf(&sg_out[1], pad, sizeof pad);

	ret = crypto_blkcipher_decrypt(&desc, sg_out, sg_in, encrypted_datalen);
	crypto_free_blkcipher(desc.tfm);
	if (ret < 0)
		goto out;
	dump_decrypted_data(epayload);
out:
	return ret;
}
