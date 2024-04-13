void r_pkcs7_free_attribute (RPKCS7Attribute* attribute) {
	if (attribute) {
		r_asn1_free_object (attribute->data);
		r_asn1_free_string (attribute->oid);
		free (attribute);
	}
}
