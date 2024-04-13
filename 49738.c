static void twofish_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	struct twofish_ctx *ctx = crypto_tfm_ctx(tfm);
	const __le32 *src = (const __le32 *)in;
	__le32 *dst = (__le32 *)out;
  
	/* The four 32-bit chunks of the text. */
	u32 a, b, c, d;
	
	/* Temporaries used by the round function. */
	u32 x, y;
	
	/* Input whitening and packing. */
	INPACK (0, c, 4);
	INPACK (1, d, 5);
	INPACK (2, a, 6);
	INPACK (3, b, 7);
	
	/* Encryption Feistel cycles. */
	DECCYCLE (7);
	DECCYCLE (6);
	DECCYCLE (5);
	DECCYCLE (4);
	DECCYCLE (3);
	DECCYCLE (2);
	DECCYCLE (1);
	DECCYCLE (0);

	/* Output whitening and unpacking. */
	OUTUNPACK (0, a, 0);
	OUTUNPACK (1, b, 1);
	OUTUNPACK (2, c, 2);
	OUTUNPACK (3, d, 3);

}
