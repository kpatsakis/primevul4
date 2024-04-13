char *r_pkcs7_cms_dump (RCMS* container) {
	RPKCS7SignedData *sd;
	ut32 i, length, p = 0;
	int r;
	char *buffer = NULL, *tmp = NULL;
	if (!container) {
		return NULL;
	}
	sd = &container->signedData;
	length = 2048 + (container->signedData.certificates.length * 1024);
	if(!length) {
		return NULL;
	}
	buffer = (char*) calloc (1, length);
	if (!buffer) {
		return NULL;
	}
	r = snprintf (buffer, length, "signedData\n  Version: %u\n  Digest Algorithms:\n", sd->version);
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (buffer);
		return NULL;
	}
	if (container->signedData.digestAlgorithms.elements) {
		for (i = 0; i < container->signedData.digestAlgorithms.length; ++i) {
			if (container->signedData.digestAlgorithms.elements[i]) {
				RASN1String *s = container->signedData.digestAlgorithms.elements[i]->algorithm;
				r = snprintf (buffer + p, length - p, "    %s\n", s ? s->string : "Missing");
				p += (ut32) r;
				if (r < 0 || length <= p) {
					free (buffer);
					return NULL;
				}
			}
		}
	}
	r = snprintf (buffer + p, length - p, "  Certificates: %u\n", container->signedData.certificates.length);
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (buffer);
		return NULL;
	}
	for (i = 0; i < container->signedData.certificates.length; ++i) {
		if (length <= p || !(tmp = r_x509_certificate_dump (container->signedData.certificates.elements[i], buffer + p, length - p, "    "))) {
			free (buffer);
			return NULL;
		}
		p = tmp - buffer;
	}

	for (i = 0; i < container->signedData.crls.length; ++i) {
		if (length <= p || !(tmp = r_x509_crl_dump (container->signedData.crls.elements[i], buffer + p, length - p, "    "))) {
			free (buffer);
			return NULL;
		}
		p = tmp - buffer;
	}
	p = tmp - buffer;
	if (length <= p) {
		free (buffer);
		return NULL;
	}	
	r = snprintf (buffer + p, length - p, "  SignerInfos:\n");
	p += (ut32) r;
	if (r < 0 || length <= p) {
		free (buffer);
		return NULL;
	}
	if (container->signedData.signerinfos.elements) {
		for (i = 0; i < container->signedData.signerinfos.length; ++i) {
			if (length <= p || !(tmp = r_x509_signedinfo_dump (container->signedData.signerinfos.elements[i], buffer + p, length - p, "    "))) {
				free (buffer);
				return NULL;
			}
			p = tmp - buffer;
		}
	}
	return buffer;
}
