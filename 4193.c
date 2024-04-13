njs_array_prototype_push(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t      length;
    njs_int_t    ret;
    njs_uint_t   i;
    njs_array_t  *array;
    njs_value_t  *this;

    length = 0;
    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (njs_is_fast_array(this)) {
        array = njs_array(this);

        if (nargs != 0) {
            ret = njs_array_expand(vm, array, 0, nargs);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }

            for (i = 1; i < nargs; i++) {
                /* GC: njs_retain(&args[i]); */
                array->start[array->length++] = args[i];
            }
        }

        njs_set_number(&vm->retval, array->length);

        return NJS_OK;
    }

    ret = njs_object_length(vm, this, &length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (njs_slow_path((length + nargs - 1) > NJS_MAX_LENGTH)) {
        njs_type_error(vm, "Invalid length");
        return NJS_ERROR;
    }

    for (i = 1; i < nargs; i++) {
        ret = njs_value_property_i64_set(vm, this, length++, &args[i]);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

    ret = njs_object_length_set(vm, this, length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    njs_set_number(&vm->retval, length);

    return NJS_OK;
}