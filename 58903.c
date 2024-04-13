void r_pkcs7_free_certificaterevocationlists (RPKCS7CertificateRevocationLists *crls) {
	ut32 i;
	if (crls) {
		for (i = 0; i < crls->length; ++i) {
			r_x509_free_crl (crls->elements[i]);
			crls->elements[i] = NULL;
		}
		R_FREE (crls->elements);
	}
}
