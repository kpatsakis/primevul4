njs_array_string_add(njs_vm_t *vm, njs_array_t *array, const u_char *start,
    size_t size, size_t length)
{
    njs_int_t  ret;

    ret = njs_array_expand(vm, array, 0, 1);

    if (njs_fast_path(ret == NJS_OK)) {
        return njs_string_new(vm, &array->start[array->length++], start, size,
                              length);
    }

    return ret;
}