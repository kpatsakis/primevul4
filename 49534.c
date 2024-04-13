static void camellia_setup_tail(u32 *subkey, u32 *subL, u32 *subR, int max)
{
	u32 dw, tl, tr;
	u32 kw4l, kw4r;

	/* absorb kw2 to other subkeys */
	/* round 2 */
	subL[3] ^= subL[1]; subR[3] ^= subR[1];
	/* round 4 */
	subL[5] ^= subL[1]; subR[5] ^= subR[1];
	/* round 6 */
	subL[7] ^= subL[1]; subR[7] ^= subR[1];
	subL[1] ^= subR[1] & ~subR[9];
	dw = subL[1] & subL[9];
	subR[1] ^= rol32(dw, 1); /* modified for FLinv(kl2) */
	/* round 8 */
	subL[11] ^= subL[1]; subR[11] ^= subR[1];
	/* round 10 */
	subL[13] ^= subL[1]; subR[13] ^= subR[1];
	/* round 12 */
	subL[15] ^= subL[1]; subR[15] ^= subR[1];
	subL[1] ^= subR[1] & ~subR[17];
	dw = subL[1] & subL[17];
	subR[1] ^= rol32(dw, 1); /* modified for FLinv(kl4) */
	/* round 14 */
	subL[19] ^= subL[1]; subR[19] ^= subR[1];
	/* round 16 */
	subL[21] ^= subL[1]; subR[21] ^= subR[1];
	/* round 18 */
	subL[23] ^= subL[1]; subR[23] ^= subR[1];
	if (max == 24) {
		/* kw3 */
		subL[24] ^= subL[1]; subR[24] ^= subR[1];

	/* absorb kw4 to other subkeys */
		kw4l = subL[25]; kw4r = subR[25];
	} else {
		subL[1] ^= subR[1] & ~subR[25];
		dw = subL[1] & subL[25];
		subR[1] ^= rol32(dw, 1); /* modified for FLinv(kl6) */
		/* round 20 */
		subL[27] ^= subL[1]; subR[27] ^= subR[1];
		/* round 22 */
		subL[29] ^= subL[1]; subR[29] ^= subR[1];
		/* round 24 */
		subL[31] ^= subL[1]; subR[31] ^= subR[1];
		/* kw3 */
		subL[32] ^= subL[1]; subR[32] ^= subR[1];

	/* absorb kw4 to other subkeys */
		kw4l = subL[33]; kw4r = subR[33];
		/* round 23 */
		subL[30] ^= kw4l; subR[30] ^= kw4r;
		/* round 21 */
		subL[28] ^= kw4l; subR[28] ^= kw4r;
		/* round 19 */
		subL[26] ^= kw4l; subR[26] ^= kw4r;
		kw4l ^= kw4r & ~subR[24];
		dw = kw4l & subL[24];
		kw4r ^= rol32(dw, 1); /* modified for FL(kl5) */
	}
	/* round 17 */
	subL[22] ^= kw4l; subR[22] ^= kw4r;
	/* round 15 */
	subL[20] ^= kw4l; subR[20] ^= kw4r;
	/* round 13 */
	subL[18] ^= kw4l; subR[18] ^= kw4r;
	kw4l ^= kw4r & ~subR[16];
	dw = kw4l & subL[16];
	kw4r ^= rol32(dw, 1); /* modified for FL(kl3) */
	/* round 11 */
	subL[14] ^= kw4l; subR[14] ^= kw4r;
	/* round 9 */
	subL[12] ^= kw4l; subR[12] ^= kw4r;
	/* round 7 */
	subL[10] ^= kw4l; subR[10] ^= kw4r;
	kw4l ^= kw4r & ~subR[8];
	dw = kw4l & subL[8];
	kw4r ^= rol32(dw, 1); /* modified for FL(kl1) */
	/* round 5 */
	subL[6] ^= kw4l; subR[6] ^= kw4r;
	/* round 3 */
	subL[4] ^= kw4l; subR[4] ^= kw4r;
	/* round 1 */
	subL[2] ^= kw4l; subR[2] ^= kw4r;
	/* kw1 */
	subL[0] ^= kw4l; subR[0] ^= kw4r;

	/* key XOR is end of F-function */
	SUBKEY_L(0) = subL[0] ^ subL[2];/* kw1 */
	SUBKEY_R(0) = subR[0] ^ subR[2];
	SUBKEY_L(2) = subL[3];       /* round 1 */
	SUBKEY_R(2) = subR[3];
	SUBKEY_L(3) = subL[2] ^ subL[4]; /* round 2 */
	SUBKEY_R(3) = subR[2] ^ subR[4];
	SUBKEY_L(4) = subL[3] ^ subL[5]; /* round 3 */
	SUBKEY_R(4) = subR[3] ^ subR[5];
	SUBKEY_L(5) = subL[4] ^ subL[6]; /* round 4 */
	SUBKEY_R(5) = subR[4] ^ subR[6];
	SUBKEY_L(6) = subL[5] ^ subL[7]; /* round 5 */
	SUBKEY_R(6) = subR[5] ^ subR[7];
	tl = subL[10] ^ (subR[10] & ~subR[8]);
	dw = tl & subL[8];  /* FL(kl1) */
	tr = subR[10] ^ rol32(dw, 1);
	SUBKEY_L(7) = subL[6] ^ tl; /* round 6 */
	SUBKEY_R(7) = subR[6] ^ tr;
	SUBKEY_L(8) = subL[8];       /* FL(kl1) */
	SUBKEY_R(8) = subR[8];
	SUBKEY_L(9) = subL[9];       /* FLinv(kl2) */
	SUBKEY_R(9) = subR[9];
	tl = subL[7] ^ (subR[7] & ~subR[9]);
	dw = tl & subL[9];  /* FLinv(kl2) */
	tr = subR[7] ^ rol32(dw, 1);
	SUBKEY_L(10) = tl ^ subL[11]; /* round 7 */
	SUBKEY_R(10) = tr ^ subR[11];
	SUBKEY_L(11) = subL[10] ^ subL[12]; /* round 8 */
	SUBKEY_R(11) = subR[10] ^ subR[12];
	SUBKEY_L(12) = subL[11] ^ subL[13]; /* round 9 */
	SUBKEY_R(12) = subR[11] ^ subR[13];
	SUBKEY_L(13) = subL[12] ^ subL[14]; /* round 10 */
	SUBKEY_R(13) = subR[12] ^ subR[14];
	SUBKEY_L(14) = subL[13] ^ subL[15]; /* round 11 */
	SUBKEY_R(14) = subR[13] ^ subR[15];
	tl = subL[18] ^ (subR[18] & ~subR[16]);
	dw = tl & subL[16]; /* FL(kl3) */
	tr = subR[18] ^ rol32(dw, 1);
	SUBKEY_L(15) = subL[14] ^ tl; /* round 12 */
	SUBKEY_R(15) = subR[14] ^ tr;
	SUBKEY_L(16) = subL[16];     /* FL(kl3) */
	SUBKEY_R(16) = subR[16];
	SUBKEY_L(17) = subL[17];     /* FLinv(kl4) */
	SUBKEY_R(17) = subR[17];
	tl = subL[15] ^ (subR[15] & ~subR[17]);
	dw = tl & subL[17]; /* FLinv(kl4) */
	tr = subR[15] ^ rol32(dw, 1);
	SUBKEY_L(18) = tl ^ subL[19]; /* round 13 */
	SUBKEY_R(18) = tr ^ subR[19];
	SUBKEY_L(19) = subL[18] ^ subL[20]; /* round 14 */
	SUBKEY_R(19) = subR[18] ^ subR[20];
	SUBKEY_L(20) = subL[19] ^ subL[21]; /* round 15 */
	SUBKEY_R(20) = subR[19] ^ subR[21];
	SUBKEY_L(21) = subL[20] ^ subL[22]; /* round 16 */
	SUBKEY_R(21) = subR[20] ^ subR[22];
	SUBKEY_L(22) = subL[21] ^ subL[23]; /* round 17 */
	SUBKEY_R(22) = subR[21] ^ subR[23];
	if (max == 24) {
		SUBKEY_L(23) = subL[22];     /* round 18 */
		SUBKEY_R(23) = subR[22];
		SUBKEY_L(24) = subL[24] ^ subL[23]; /* kw3 */
		SUBKEY_R(24) = subR[24] ^ subR[23];
	} else {
		tl = subL[26] ^ (subR[26] & ~subR[24]);
		dw = tl & subL[24]; /* FL(kl5) */
		tr = subR[26] ^ rol32(dw, 1);
		SUBKEY_L(23) = subL[22] ^ tl; /* round 18 */
		SUBKEY_R(23) = subR[22] ^ tr;
		SUBKEY_L(24) = subL[24];     /* FL(kl5) */
		SUBKEY_R(24) = subR[24];
		SUBKEY_L(25) = subL[25];     /* FLinv(kl6) */
		SUBKEY_R(25) = subR[25];
		tl = subL[23] ^ (subR[23] & ~subR[25]);
		dw = tl & subL[25]; /* FLinv(kl6) */
		tr = subR[23] ^ rol32(dw, 1);
		SUBKEY_L(26) = tl ^ subL[27]; /* round 19 */
		SUBKEY_R(26) = tr ^ subR[27];
		SUBKEY_L(27) = subL[26] ^ subL[28]; /* round 20 */
		SUBKEY_R(27) = subR[26] ^ subR[28];
		SUBKEY_L(28) = subL[27] ^ subL[29]; /* round 21 */
		SUBKEY_R(28) = subR[27] ^ subR[29];
		SUBKEY_L(29) = subL[28] ^ subL[30]; /* round 22 */
		SUBKEY_R(29) = subR[28] ^ subR[30];
		SUBKEY_L(30) = subL[29] ^ subL[31]; /* round 23 */
		SUBKEY_R(30) = subR[29] ^ subR[31];
		SUBKEY_L(31) = subL[30];     /* round 24 */
		SUBKEY_R(31) = subR[30];
		SUBKEY_L(32) = subL[32] ^ subL[31]; /* kw3 */
		SUBKEY_R(32) = subR[32] ^ subR[31];
	}
}
