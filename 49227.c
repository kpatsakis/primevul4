static void aesni_gcm_enc_avx(void *ctx, u8 *out,
			const u8 *in, unsigned long plaintext_len, u8 *iv,
			u8 *hash_subkey, const u8 *aad, unsigned long aad_len,
			u8 *auth_tag, unsigned long auth_tag_len)
{
	if (plaintext_len < AVX_GEN2_OPTSIZE) {
		aesni_gcm_enc(ctx, out, in, plaintext_len, iv, hash_subkey, aad,
				aad_len, auth_tag, auth_tag_len);
	} else {
		aesni_gcm_precomp_avx_gen2(ctx, hash_subkey);
		aesni_gcm_enc_avx_gen2(ctx, out, in, plaintext_len, iv, aad,
					aad_len, auth_tag, auth_tag_len);
	}
}
