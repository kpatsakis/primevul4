bool r_pkcs7_parse_issuerandserialnumber (RPKCS7IssuerAndSerialNumber* iasu, RASN1Object *object) {
	if (!iasu || !object || object->list.length != 2) {
		return false;
	}

	r_x509_parse_name (&iasu->issuer, object->list.objects[0]);
	R_PTR_MOVE (iasu->serialNumber, object->list.objects[1]);

	return true;
}
