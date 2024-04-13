bool r_pkcs7_parse_signeddata (RPKCS7SignedData *sd, RASN1Object *object) {
	RASN1Object **elems;
	ut32 shift = 3;
	if (!sd || !object || object->list.length < 4) {
		return false;
	}
	memset (sd, 0, sizeof (RPKCS7SignedData));
	elems = object->list.objects;
	sd->version = (ut32) elems[0]->sector[0]; 
	r_pkcs7_parse_digestalgorithmidentifier (&sd->digestAlgorithms, elems[1]);
	r_pkcs7_parse_contentinfo (&sd->contentInfo, elems[2]);
	if (shift < object->list.length && elems[shift]->klass == CLASS_CONTEXT && elems[shift]->tag == 0) {
		r_pkcs7_parse_extendedcertificatesandcertificates (&sd->certificates, elems[shift]);
		shift++;
	}
	if (shift < object->list.length && elems[shift]->klass == CLASS_CONTEXT && elems[shift]->tag == 1) {
		r_pkcs7_parse_certificaterevocationlists (&sd->crls, elems[shift]);
		shift++;
	}
	if (shift < object->list.length) {
		r_pkcs7_parse_signerinfos (&sd->signerinfos, elems[shift]);
	}
	return true;
}
