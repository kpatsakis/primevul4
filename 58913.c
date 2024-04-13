bool r_pkcs7_parse_attributes (RPKCS7Attributes* attributes, RASN1Object *object) {
	ut32 i;
	if (!attributes || !object || !object->list.length) {
		return false;
	}

	attributes->length = object->list.length;
	if (attributes->length > 0) {
		attributes->elements = R_NEWS0 (RPKCS7Attribute*, attributes->length);
		if (!attributes->elements) {
			attributes->length = 0;
			return false;
		}
		for (i = 0; i < object->list.length; ++i) {
			attributes->elements[i] = r_pkcs7_parse_attribute (object->list.objects[i]);
		}
	}
	return true;
}
