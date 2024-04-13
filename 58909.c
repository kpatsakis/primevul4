void r_pkcs7_free_signeddata (RPKCS7SignedData* sd) {
	if (sd) {
		r_pkcs7_free_digestalgorithmidentifier (&sd->digestAlgorithms);
		r_pkcs7_free_contentinfo (&sd->contentInfo);
		r_pkcs7_free_extendedcertificatesandcertificates (&sd->certificates);
		r_pkcs7_free_certificaterevocationlists (&sd->crls);
		r_pkcs7_free_signerinfos (&sd->signerinfos);
	}
}
