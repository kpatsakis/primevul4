void r_pkcs7_free_contentinfo (RPKCS7ContentInfo* ci) {
	if (ci) {
		r_asn1_free_object (ci->content);
		r_asn1_free_string (ci->contentType);
	}
}
