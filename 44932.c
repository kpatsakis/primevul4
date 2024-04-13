static int __lookup_one_len(const char *name, struct qstr *this,
		struct dentry *base, int len)
{
	unsigned long hash;
	unsigned int c;

	this->name = name;
	this->len = len;
	if (!len)
		return -EACCES;

	hash = init_name_hash();
	while (len--) {
		c = *(const unsigned char *)name++;
		if (c == '/' || c == '\0')
			return -EACCES;
		hash = partial_name_hash(c, hash);
	}
	this->hash = end_name_hash(hash);
	return 0;
}
