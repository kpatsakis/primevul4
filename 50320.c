Strcopy(Str x, Str y)
{
    STR_LENGTH_CHECK(x);
    STR_LENGTH_CHECK(y);
    if (x->area_size < y->length + 1) {
	GC_free(x->ptr);
	x->ptr = GC_MALLOC_ATOMIC(y->length + 1);
	x->area_size = y->length + 1;
    }
    bcopy((void *)y->ptr, (void *)x->ptr, y->length + 1);
    x->length = y->length;
}
