static int evm_protected_xattr(const char *req_xattr_name)
{
	char **xattrname;
	int namelen;
	int found = 0;

	namelen = strlen(req_xattr_name);
	for (xattrname = evm_config_xattrnames; *xattrname != NULL; xattrname++) {
		if ((strlen(*xattrname) == namelen)
		    && (strncmp(req_xattr_name, *xattrname, namelen) == 0)) {
			found = 1;
			break;
		}
		if (strncmp(req_xattr_name,
			    *xattrname + XATTR_SECURITY_PREFIX_LEN,
			    strlen(req_xattr_name)) == 0) {
			found = 1;
			break;
		}
	}
	return found;
}
