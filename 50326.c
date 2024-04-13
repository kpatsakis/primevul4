Strfree(Str x)
{
    GC_free(x->ptr);
    GC_free(x);
}
