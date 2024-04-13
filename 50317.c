Strcat_charp_n(Str x, const char *y, int n)
{
    int newlen;

    STR_LENGTH_CHECK(x);
    if (y == NULL)
	return;
    newlen = x->length + n + 1;
    if (x->area_size < newlen) {
	char *old = x->ptr;
	newlen = newlen * 3 / 2;
	x->ptr = GC_MALLOC_ATOMIC(newlen);
	x->area_size = newlen;
	bcopy((void *)old, (void *)x->ptr, x->length);
	GC_free(old);
    }
    bcopy((void *)y, (void *)&x->ptr[x->length], n);
    x->length += n;
    x->ptr[x->length] = '\0';
}
