RPKCS7Attribute* r_pkcs7_parse_attribute (RASN1Object *object) {
	RPKCS7Attribute* attribute;
	if (!object || object->list.length < 1) {
		return NULL;
	}
	attribute = R_NEW0 (RPKCS7Attribute);
	if (!attribute) {
		return NULL;
	}
	if (object->list.objects[0]) {
		attribute->oid = r_asn1_stringify_oid (object->list.objects[0]->sector, object->list.objects[0]->length);
	}
	if (object->list.length == 2) {
		R_PTR_MOVE (attribute->data, object->list.objects[1]);
	}
	return attribute;
}
