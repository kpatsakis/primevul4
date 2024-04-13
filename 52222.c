bid_keyword(const char *p,  ssize_t len)
{
	static const char *keys_c[] = {
		"content", "contents", "cksum", NULL
	};
	static const char *keys_df[] = {
		"device", "flags", NULL
	};
	static const char *keys_g[] = {
		"gid", "gname", NULL
	};
	static const char *keys_il[] = {
		"ignore", "inode", "link", NULL
	};
	static const char *keys_m[] = {
		"md5", "md5digest", "mode", NULL
	};
	static const char *keys_no[] = {
		"nlink", "nochange", "optional", NULL
	};
	static const char *keys_r[] = {
		"resdevice", "rmd160", "rmd160digest", NULL
	};
	static const char *keys_s[] = {
		"sha1", "sha1digest",
		"sha256", "sha256digest",
		"sha384", "sha384digest",
		"sha512", "sha512digest",
		"size", NULL
	};
	static const char *keys_t[] = {
		"tags", "time", "type", NULL
	};
	static const char *keys_u[] = {
		"uid", "uname",	NULL
	};
	const char **keys;
	int i;

	switch (*p) {
	case 'c': keys = keys_c; break;
	case 'd': case 'f': keys = keys_df; break;
	case 'g': keys = keys_g; break;
	case 'i': case 'l': keys = keys_il; break;
	case 'm': keys = keys_m; break;
	case 'n': case 'o': keys = keys_no; break;
	case 'r': keys = keys_r; break;
	case 's': keys = keys_s; break;
	case 't': keys = keys_t; break;
	case 'u': keys = keys_u; break;
	default: return (0);/* Unknown key */
	}

	for (i = 0; keys[i] != NULL; i++) {
		int l = bid_keycmp(p, keys[i], len);
		if (l > 0)
			return (l);
	}
	return (0);/* Unknown key */
}
