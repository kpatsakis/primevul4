njs_array_constructor(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    double       num;
    uint32_t     size;
    njs_value_t  *value;
    njs_array_t  *array;

    args = &args[1];
    size = nargs - 1;

    if (size == 1 && njs_is_number(&args[0])) {
        num = njs_number(&args[0]);
        size = (uint32_t) njs_number_to_length(num);

        if ((double) size != num) {
            njs_range_error(vm, "Invalid array length");
            return NJS_ERROR;
        }

        args = NULL;
    }

    array = njs_array_alloc(vm, size <= NJS_ARRAY_FLAT_MAX_LENGTH,
                            size, NJS_ARRAY_SPARE);

    if (njs_fast_path(array != NULL)) {

        if (array->object.fast_array) {
            value = array->start;

            if (args == NULL) {
                while (size != 0) {
                    njs_set_invalid(value);
                    value++;
                    size--;
                }

            } else {
                while (size != 0) {
                    njs_retain(args);
                    *value++ = *args++;
                    size--;
                }
            }
        }

        njs_set_array(&vm->retval, array);

        return NJS_OK;
    }

    return NJS_ERROR;
}