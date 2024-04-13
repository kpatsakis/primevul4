njs_array_add(njs_vm_t *vm, njs_array_t *array, njs_value_t *value)
{
    njs_int_t  ret;

    ret = njs_array_expand(vm, array, 0, 1);

    if (njs_fast_path(ret == NJS_OK)) {
        /* GC: retain value. */
        array->start[array->length++] = *value;
    }

    return ret;
}