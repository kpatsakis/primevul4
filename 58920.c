bool r_pkcs7_parse_signerinfo (RPKCS7SignerInfo* si, RASN1Object *object) {
	RASN1Object **elems;
	ut32 shift = 3;
	if (!si || !object || object->list.length < 5) {
		return false;
	}
	elems = object->list.objects;
	si->version = (ut32) elems[0]->sector[0];
	r_pkcs7_parse_issuerandserialnumber (&si->issuerAndSerialNumber, elems[1]);
	r_x509_parse_algorithmidentifier (&si->digestAlgorithm, elems[2]);
	if (shift < object->list.length && elems[shift]->klass == CLASS_CONTEXT && elems[shift]->tag == 0) {
		r_pkcs7_parse_attributes (&si->authenticatedAttributes, elems[shift]);
		shift++;
	}
	if (shift < object->list.length) {
		r_x509_parse_algorithmidentifier (&si->digestEncryptionAlgorithm, elems[shift]);
		shift++;
	}
	if (shift < object->list.length) {
		R_PTR_MOVE (si->encryptedDigest, object->list.objects[shift]);
		shift++;
	}
	if (shift < object->list.length && elems[shift]->klass == CLASS_CONTEXT && elems[shift]->tag == 1) {
		r_pkcs7_parse_attributes (&si->unauthenticatedAttributes, elems[shift]);
	}
	return true;
}
