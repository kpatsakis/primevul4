static int cuse_parse_one(char **pp, char *end, char **keyp, char **valp)
{
	char *p = *pp;
	char *key, *val;

	while (p < end && *p == '\0')
		p++;
	if (p == end)
		return 0;

	if (end[-1] != '\0') {
		printk(KERN_ERR "CUSE: info not properly terminated\n");
		return -EINVAL;
	}

	key = val = p;
	p += strlen(p);

	if (valp) {
		strsep(&val, "=");
		if (!val)
			val = key + strlen(key);
		key = strstrip(key);
		val = strstrip(val);
	} else
		key = strstrip(key);

	if (!strlen(key)) {
		printk(KERN_ERR "CUSE: zero length info key specified\n");
		return -EINVAL;
	}

	*pp = p;
	*keyp = key;
	if (valp)
		*valp = val;

	return 1;
}
