bool r_pkcs7_parse_certificaterevocationlists (RPKCS7CertificateRevocationLists *crls, RASN1Object *object) {
	ut32 i;
	if (!crls && !object) {
		return false;
	}
	if (object->list.length > 0) {
		crls->elements = (RX509CertificateRevocationList **) calloc (object->list.length, sizeof (RX509CertificateRevocationList*));
		if (!crls->elements) {
			return false;
		}
		crls->length = object->list.length;
		for (i = 0; i < crls->length; ++i) {
			crls->elements[i] = r_x509_parse_crl (object->list.objects[i]);
		}
	}
	return true;
}
