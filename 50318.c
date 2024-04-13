Strcat_m_charp(Str x, ...)
{
    va_list ap;
    char *p;

    va_start(ap, x);
    while ((p = va_arg(ap, char *)) != NULL)
	 Strcat_charp_n(x, p, strlen(p));
}
