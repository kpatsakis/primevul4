void strtolower(char *i)
{
    if (!i)
	return;
    while (*i) {
	*i = (char) TOLOWER(*i);
	i++;
    }
}
