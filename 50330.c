Strnew_charp(const char *p)
{
    Str x;
    int n;

    if (p == NULL)
	return Strnew();
    x = GC_MALLOC(sizeof(struct _Str));
    n = strlen(p) + 1;
    x->ptr = GC_MALLOC_ATOMIC(n);
    x->area_size = n;
    x->length = n - 1;
    bcopy((void *)p, (void *)x->ptr, n);
    return x;
}
