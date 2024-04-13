Strcopy_charp_n(Str x, const char *y, int n)
{
    int len = n;

    STR_LENGTH_CHECK(x);
    if (y == NULL) {
	x->length = 0;
	return;
    }
    if (x->area_size < len + 1) {
	GC_free(x->ptr);
	x->ptr = GC_MALLOC_ATOMIC(len + 1);
	x->area_size = len + 1;
    }
    bcopy((void *)y, (void *)x->ptr, n);
    x->ptr[n] = '\0';
    x->length = n;
}
