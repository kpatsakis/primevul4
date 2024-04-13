njs_array_prototype_unshift(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    double       idx;
    int64_t      from, to, length;
    njs_int_t    ret;
    njs_uint_t   n;
    njs_array_t  *array, *keys;
    njs_value_t  *this, entry;

    this = njs_argument(args, 0);
    length = 0;
    n = nargs - 1;

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (njs_fast_path(njs_is_fast_array(this))) {
        array = njs_array(this);

        if (n != 0) {
            ret = njs_array_expand(vm, array, n, 0);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }

            array->length += n;
            n = nargs;

            do {
                n--;
                /* GC: njs_retain(&args[n]); */
                array->start--;
                array->start[0] = args[n];
            } while (n > 1);
        }

        njs_set_number(&vm->retval, array->length);

        return NJS_OK;
    }

    ret = njs_object_length(vm, this, &length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (n == 0) {
        goto done;
    }

    if (njs_slow_path((length + n) > NJS_MAX_LENGTH)) {
        njs_type_error(vm, "Invalid length");
        return NJS_ERROR;
    }

    if (!njs_fast_object(length)) {
        keys = njs_array_indices(vm, this);
        if (njs_slow_path(keys == NULL)) {
            return NJS_ERROR;
        }

        from = keys->length;

        while (from > 0) {
            ret = njs_value_property_delete(vm, this, &keys->start[--from],
                                            &entry);
            if (njs_slow_path(ret == NJS_ERROR)) {
                njs_array_destroy(vm, keys);
                return ret;
            }

            if (ret == NJS_OK) {
                idx = njs_string_to_index(&keys->start[from]) + n;

                ret = njs_value_property_i64_set(vm, this, idx, &entry);
                if (njs_slow_path(ret == NJS_ERROR)) {
                    njs_array_destroy(vm, keys);
                    return ret;
                }
            }
        }

        njs_array_destroy(vm, keys);

        length += n;

        goto copy;
    }

    from = length;
    length += n;
    to = length;

    while (from > 0) {
        ret = njs_value_property_i64_delete(vm, this, --from, &entry);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        to--;

        if (ret == NJS_OK) {
            ret = njs_value_property_i64_set(vm, this, to, &entry);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return ret;
            }
        }
    }

copy:

    for (n = 1; n < nargs; n++) {
        ret = njs_value_property_i64_set(vm, this, n - 1, &args[n]);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

done:

    ret = njs_object_length_set(vm, this, length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    njs_set_number(&vm->retval, length);

    return NJS_OK;
}