void r_pkcs7_free_signerinfo (RPKCS7SignerInfo* si) {
	if (si) {
		r_pkcs7_free_issuerandserialnumber (&si->issuerAndSerialNumber);
		r_x509_free_algorithmidentifier (&si->digestAlgorithm);
		r_pkcs7_free_attributes (&si->authenticatedAttributes);
		r_x509_free_algorithmidentifier (&si->digestEncryptionAlgorithm);
		r_asn1_free_object (si->encryptedDigest);
		r_pkcs7_free_attributes (&si->unauthenticatedAttributes);
		free (si);
	}
}
