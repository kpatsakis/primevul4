bool r_pkcs7_parse_extendedcertificatesandcertificates (RPKCS7ExtendedCertificatesAndCertificates *ecac, RASN1Object *object) {
	ut32 i;
	if (!ecac && !object) {
		return false;
	}
	if (object->list.length > 0) {
		ecac->elements = (RX509Certificate **) calloc (object->list.length, sizeof (RX509Certificate*));
		if (!ecac->elements) {
			return false;
		}
		ecac->length = object->list.length;
		for (i = 0; i < ecac->length; ++i) {
			ecac->elements[i] = r_x509_parse_certificate (object->list.objects[i]);
			object->list.objects[i] = NULL;
		}
	}
	return true;
}
