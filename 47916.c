static void stripnewline(char *x)
{
	size_t l = strlen(x);
	if (l && x[l-1] == '\n')
		x[l-1] = '\0';
}
