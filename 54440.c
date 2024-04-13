static void addClassName(const char *prefix,
			 const char *actual,
			 size_t length)
{
    size_t offset = strlen(prefix);
    size_t have = (unsigned) (Style_className_end - Style_className);
    size_t need = (offset + length + 1);

    if ((have + need) >= Style_className_len) {
	Style_className_len += 1024 + 2 * (have + need);
	if (Style_className == 0) {
	    Style_className = typeMallocn(char, Style_className_len);
	} else {
	    Style_className = typeRealloc(char, Style_className, Style_className_len);
	}
	if (Style_className == NULL)
	    outofmem(__FILE__, "addClassName");
	Style_className_end = Style_className + have;
    }
    if (offset)
	strcpy(Style_className_end, prefix);
    if (length)
	memcpy(Style_className_end + offset, actual, length);
    Style_className_end[offset + length] = '\0';
    strtolower(Style_className_end);

    Style_className_end += (offset + length);
}
