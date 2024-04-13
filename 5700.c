static void trim_key()
{
	char *p;
	int i;

	for (i=0, p=booth_conf->authkey; i < booth_conf->authkey_len; i++, p++)
		if (!isascii(*p))
			return;

	p = booth_conf->authkey;
	while (booth_conf->authkey_len > 0 && isspace(*p)) {
		p++;
		booth_conf->authkey_len--;
	}
	memmove(booth_conf->authkey, p, booth_conf->authkey_len);

	p = booth_conf->authkey + booth_conf->authkey_len - 1;
	while (booth_conf->authkey_len > 0 && isspace(*p)) {
		booth_conf->authkey_len--;
		p--;
	}
}