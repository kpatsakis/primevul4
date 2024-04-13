Strcopy_charp(Str x, const char *y)
{
    int len;

    STR_LENGTH_CHECK(x);
    if (y == NULL) {
	x->length = 0;
	return;
    }
    len = strlen(y);
    if (x->area_size < len + 1) {
	GC_free(x->ptr);
	x->ptr = GC_MALLOC_ATOMIC(len + 1);
	x->area_size = len + 1;
    }
    bcopy((void *)y, (void *)x->ptr, len + 1);
    x->length = len;
}
