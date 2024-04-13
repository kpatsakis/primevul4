acl_print(netdissect_options *ndo,
          u_char *s, int maxsize, u_char *end)
{
	int pos, neg, acl;
	int n, i;
	char *user;
	char fmt[1024];

	if ((user = (char *)malloc(maxsize)) == NULL)
		return;

	if (sscanf((char *) s, "%d %d\n%n", &pos, &neg, &n) != 2)
		goto finish;

	s += n;

	if (s > end)
		goto finish;

	/*
	 * This wacky order preserves the order used by the "fs" command
	 */

#define ACLOUT(acl) \
	ND_PRINT((ndo, "%s%s%s%s%s%s%s", \
	          acl & PRSFS_READ       ? "r" : "", \
	          acl & PRSFS_LOOKUP     ? "l" : "", \
	          acl & PRSFS_INSERT     ? "i" : "", \
	          acl & PRSFS_DELETE     ? "d" : "", \
	          acl & PRSFS_WRITE      ? "w" : "", \
	          acl & PRSFS_LOCK       ? "k" : "", \
	          acl & PRSFS_ADMINISTER ? "a" : ""));

	for (i = 0; i < pos; i++) {
		snprintf(fmt, sizeof(fmt), "%%%ds %%d\n%%n", maxsize - 1);
		if (sscanf((char *) s, fmt, user, &acl, &n) != 2)
			goto finish;
		s += n;
		ND_PRINT((ndo, " +{"));
		fn_print(ndo, (u_char *)user, NULL);
		ND_PRINT((ndo, " "));
		ACLOUT(acl);
		ND_PRINT((ndo, "}"));
		if (s > end)
			goto finish;
	}

	for (i = 0; i < neg; i++) {
		snprintf(fmt, sizeof(fmt), "%%%ds %%d\n%%n", maxsize - 1);
		if (sscanf((char *) s, fmt, user, &acl, &n) != 2)
			goto finish;
		s += n;
		ND_PRINT((ndo, " -{"));
		fn_print(ndo, (u_char *)user, NULL);
		ND_PRINT((ndo, " "));
		ACLOUT(acl);
		ND_PRINT((ndo, "}"));
		if (s > end)
			goto finish;
	}

finish:
	free(user);
	return;
}
