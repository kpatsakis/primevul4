static int getoptions(char *c, struct trusted_key_payload *pay,
		      struct trusted_key_options *opt)
{
	substring_t args[MAX_OPT_ARGS];
	char *p = c;
	int token;
	int res;
	unsigned long handle;
	unsigned long lock;

	while ((p = strsep(&c, " \t"))) {
		if (*p == '\0' || *p == ' ' || *p == '\t')
			continue;
		token = match_token(p, key_tokens, args);

		switch (token) {
		case Opt_pcrinfo:
			opt->pcrinfo_len = strlen(args[0].from) / 2;
			if (opt->pcrinfo_len > MAX_PCRINFO_SIZE)
				return -EINVAL;
			res = hex2bin(opt->pcrinfo, args[0].from,
				      opt->pcrinfo_len);
			if (res < 0)
				return -EINVAL;
			break;
		case Opt_keyhandle:
			res = kstrtoul(args[0].from, 16, &handle);
			if (res < 0)
				return -EINVAL;
			opt->keytype = SEAL_keytype;
			opt->keyhandle = handle;
			break;
		case Opt_keyauth:
			if (strlen(args[0].from) != 2 * SHA1_DIGEST_SIZE)
				return -EINVAL;
			res = hex2bin(opt->keyauth, args[0].from,
				      SHA1_DIGEST_SIZE);
			if (res < 0)
				return -EINVAL;
			break;
		case Opt_blobauth:
			if (strlen(args[0].from) != 2 * SHA1_DIGEST_SIZE)
				return -EINVAL;
			res = hex2bin(opt->blobauth, args[0].from,
				      SHA1_DIGEST_SIZE);
			if (res < 0)
				return -EINVAL;
			break;
		case Opt_migratable:
			if (*args[0].from == '0')
				pay->migratable = 0;
			else
				return -EINVAL;
			break;
		case Opt_pcrlock:
			res = kstrtoul(args[0].from, 10, &lock);
			if (res < 0)
				return -EINVAL;
			opt->pcrlock = lock;
			break;
		default:
			return -EINVAL;
		}
	}
	return 0;
}
