Strnew_size(int n)
{
    Str x = GC_MALLOC(sizeof(struct _Str));
    x->ptr = GC_MALLOC_ATOMIC(n + 1);
    x->ptr[0] = '\0';
    x->area_size = n + 1;
    x->length = 0;
    return x;
}
