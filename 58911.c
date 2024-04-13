void r_pkcs7_free_signerinfos (RPKCS7SignerInfos *ss) {
	ut32 i;
	if (ss) {
		for (i = 0; i < ss->length; i++) {
			r_pkcs7_free_signerinfo (ss->elements[i]);
			ss->elements[i] = NULL;
		}
		R_FREE (ss->elements);
	}
}
