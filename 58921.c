char* r_x509_signedinfo_dump (RPKCS7SignerInfo *si, char* buffer, ut32 length, const char* pad) {
	RASN1String *s = NULL;
	RASN1Object *o = NULL;
	ut32 i, p;
	int r;
	char *tmp, *pad2, *pad3;
	if (!si || !buffer || !length) {
		return NULL;
	}
	if (!pad) {
		pad = "";
	}
	pad3 = r_str_newf ("%s    ", pad);
	if (!pad3) {
		return NULL;
	}
	pad2 = pad3 + 2;


	r = snprintf (buffer, length, "%sSignerInfo:\n%sVersion: v%u\n%sIssuer\n", pad, pad2, si->version + 1, pad2);
	p = (ut32) r;
	if (r < 0) {
		free (pad3);
		return NULL;
	}

	if (length <= p || !(tmp = r_x509_name_dump (&si->issuerAndSerialNumber.issuer, buffer + p, length - p, pad3))) {
		free (pad3);
		return NULL;
	}
	p = tmp - buffer;
	if ((o = si->issuerAndSerialNumber.serialNumber)) {
		s = r_asn1_stringify_integer (o->sector, o->length);
	} else {
		s = NULL;
	}
	if (length <= p) {
		free (pad3);
		return NULL;
	}
	r = snprintf (buffer + p, length - p, "%sSerial Number:\n%s%s\n", pad2, pad3, s ? s->string : "Missing");
	p += (ut32) r;
	r_asn1_free_string (s);
	if (r < 0 || length <= p) {
		free (pad3);
		return NULL;
	}
	s = si->digestAlgorithm.algorithm;
	r = snprintf (buffer + p, length - p, "%sDigest Algorithm:\n%s%s\n%sAuthenticated Attributes:\n",
				pad2, pad3, s ? s->string : "Missing", pad2);
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (pad3);
		return NULL;
	}
	for (i = 0; i < si->authenticatedAttributes.length; ++i) {
		RPKCS7Attribute* attr = si->authenticatedAttributes.elements[i];
		if (!attr) continue;
		r = snprintf (buffer + p, length - p, "%s%s: %u bytes\n",
					pad3, attr->oid ? attr->oid->string : "Missing", attr->data ? attr->data->length : 0);
		p += (ut32) r;
		if (r < 0 || length <= p) {
			free (pad3);
			return NULL;
		}
	}
	s = si->digestEncryptionAlgorithm.algorithm;
	if (length <= p) {
		free (pad3);
		return NULL;
	}
	r = snprintf (buffer + p, length - p, "%sDigest Encryption Algorithm\n%s%s\n",
				pad2, pad3, s ? s->string : "Missing");
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (pad3);
		return NULL;
	}

	r = snprintf (buffer + p, length - p, "%sEncrypted Digest: %u bytes\n", pad2, o ? o->length : 0);
	if (r < 0 || length <= p) {
		free (pad3);
		return NULL;
	}
	r = snprintf (buffer + p, length - p, "%sUnauthenticated Attributes:\n", pad2);
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (pad3);
		return NULL;
	}
	for (i = 0; i < si->unauthenticatedAttributes.length; ++i) {
		RPKCS7Attribute* attr = si->unauthenticatedAttributes.elements[i];
		if (!attr) {
			continue;
		}
		o = attr->data;
		r = snprintf (buffer + p, length - p, "%s%s: %u bytes\n",
					pad3, attr->oid ? attr->oid->string : "Missing", o ? o->length : 0);
		p += (ut32) r;
		if (r < 0 || length <= p) {
			free (pad3);
			return NULL;
		}
	}
	free (pad3);
	return buffer + p;
}
