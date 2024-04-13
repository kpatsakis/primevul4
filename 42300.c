static inline int udf_match(int len1, const unsigned char *name1, int len2,
			    const unsigned char *name2)
{
	if (len1 != len2)
		return 0;

	return !memcmp(name1, name2, len1);
}
