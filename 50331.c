Strnew_m_charp(const char *p, ...)
{
    va_list ap;
    Str r = Strnew();

    va_start(ap, p);
    while (p != NULL) {
	Strcat_charp(r, p);
	p = va_arg(ap, char *);
    }
    return r;
}
