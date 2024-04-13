bool r_pkcs7_parse_contentinfo (RPKCS7ContentInfo* ci, RASN1Object *object) {
	if (!ci || !object || object->list.length < 1 || !object->list.objects[0]) {
		return false;
	}

	ci->contentType = r_asn1_stringify_oid (object->list.objects[0]->sector, object->list.objects[0]->length);
	if (object->list.length > 1) {
		R_PTR_MOVE (ci->content, object->list.objects[1]);
	}

	return true;
}
