void r_pkcs7_free_extendedcertificatesandcertificates (RPKCS7ExtendedCertificatesAndCertificates *ecac) {
	ut32 i;
	if (ecac) {
		for (i = 0; i < ecac->length; ++i) {
			r_x509_free_certificate (ecac->elements[i]);
			ecac->elements[i] = NULL;
		}
		R_FREE (ecac->elements);
	}
}
