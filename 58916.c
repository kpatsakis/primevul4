bool r_pkcs7_parse_digestalgorithmidentifier (RPKCS7DigestAlgorithmIdentifiers *dai, RASN1Object *object) {
	ut32 i;
	if (!dai && !object) {
		return false;
	}
	if (object->list.length > 0) {
		dai->elements = (RX509AlgorithmIdentifier **) calloc (object->list.length, sizeof (RX509AlgorithmIdentifier*));
		if (!dai->elements) {
			return false;
		}
		dai->length = object->list.length;
		for (i = 0; i < dai->length; ++i) {
			dai->elements[i] = (RX509AlgorithmIdentifier *) malloc (sizeof (RX509AlgorithmIdentifier));
			if (dai->elements[i]) {
				memset (dai->elements[i], 0, sizeof (RX509AlgorithmIdentifier));
				r_x509_parse_algorithmidentifier (dai->elements[i], object->list.objects[i]);
			}
		}
	}
	return true;
}
