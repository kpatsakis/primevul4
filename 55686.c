getsumalgorithm(struct xmlattr_list *list)
{
	struct xmlattr *attr;
	int alg = CKSUM_NONE;

	for (attr = list->first; attr != NULL; attr = attr->next) {
		if (strcmp(attr->name, "style") == 0) {
			const char *v = attr->value;
			if ((v[0] == 'S' || v[0] == 's') &&
			    (v[1] == 'H' || v[1] == 'h') &&
			    (v[2] == 'A' || v[2] == 'a') &&
			    v[3] == '1' && v[4] == '\0')
				alg = CKSUM_SHA1;
			if ((v[0] == 'M' || v[0] == 'm') &&
			    (v[1] == 'D' || v[1] == 'd') &&
			    v[2] == '5' && v[3] == '\0')
				alg = CKSUM_MD5;
		}
	}
	return (alg);
}
