static int derived_key_encrypt(struct encrypted_key_payload *epayload,
			       const u8 *derived_key,
			       unsigned int derived_keylen)
{
	struct scatterlist sg_in[2];
	struct scatterlist sg_out[1];
	struct blkcipher_desc desc;
	unsigned int encrypted_datalen;
	unsigned int padlen;
	char pad[16];
	int ret;

	encrypted_datalen = roundup(epayload->decrypted_datalen, blksize);
	padlen = encrypted_datalen - epayload->decrypted_datalen;

	ret = init_blkcipher_desc(&desc, derived_key, derived_keylen,
				  epayload->iv, ivsize);
	if (ret < 0)
		goto out;
	dump_decrypted_data(epayload);

	memset(pad, 0, sizeof pad);
	sg_init_table(sg_in, 2);
	sg_set_buf(&sg_in[0], epayload->decrypted_data,
		   epayload->decrypted_datalen);
	sg_set_buf(&sg_in[1], pad, padlen);

	sg_init_table(sg_out, 1);
	sg_set_buf(sg_out, epayload->encrypted_data, encrypted_datalen);

	ret = crypto_blkcipher_encrypt(&desc, sg_out, sg_in, encrypted_datalen);
	crypto_free_blkcipher(desc.tfm);
	if (ret < 0)
		pr_err("encrypted_key: failed to encrypt (%d)\n", ret);
	else
		dump_encrypted_data(epayload, encrypted_datalen);
out:
	return ret;
}
