int asymmetric_keyid_match(const char *kid, const char *id)
{
	size_t idlen, kidlen;

	if (!kid || !id)
		return 0;

	/* make it possible to use id as in the request: "id:<id>" */
	if (strncmp(id, "id:", 3) == 0)
		id += 3;

	/* Anything after here requires a partial match on the ID string */
	idlen = strlen(id);
	kidlen = strlen(kid);
	if (idlen > kidlen)
		return 0;

	kid += kidlen - idlen;
	if (strcasecmp(id, kid) != 0)
		return 0;

	return 1;
}
