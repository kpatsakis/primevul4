void r_pkcs7_free_cms (RCMS* container) {
	if (container) {
		r_asn1_free_string (container->contentType);
		r_pkcs7_free_signeddata (&container->signedData);
		free (container);
	}
}
