void r_pkcs7_free_digestalgorithmidentifier (RPKCS7DigestAlgorithmIdentifiers *dai) {
	ut32 i;
	if (dai) {
		for (i = 0; i < dai->length; ++i) {
			if (dai->elements[i]) {
				r_x509_free_algorithmidentifier (dai->elements[i]);
				R_FREE (dai->elements[i]);
			}
		}
		R_FREE (dai->elements);
	}
}
