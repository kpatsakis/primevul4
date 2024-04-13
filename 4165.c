njs_array_of(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    uint32_t     length, i;
    njs_array_t  *array;

    length = nargs > 1 ? nargs - 1 : 0;

    array = njs_array_alloc(vm, 0, length, NJS_ARRAY_SPARE);
    if (njs_slow_path(array == NULL)) {
        return NJS_ERROR;
    }

    njs_set_array(&vm->retval, array);

    if (array->object.fast_array) {
        for (i = 0; i < length; i++) {
            array->start[i] = args[i + 1];
        }
    }

    return NJS_OK;
}