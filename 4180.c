njs_array_prototype_fill(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t       i, length, start, end;
    njs_int_t     ret;
    njs_array_t   *array;
    njs_value_t   *this, *value;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    array = NULL;

    if (njs_is_fast_array(this)) {
        array = njs_array(this);
        length = array->length;

    } else {
        ret = njs_object_length(vm, this, &length);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

    ret = njs_value_to_integer(vm, njs_arg(args, nargs, 2), &start);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    start = (start < 0) ? njs_max(length + start, 0) : njs_min(start, length);

    if (njs_is_undefined(njs_arg(args, nargs, 3))) {
        end = length;

    } else {
        ret = njs_value_to_integer(vm, njs_arg(args, nargs, 3), &end);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    end = (end < 0) ? njs_max(length + end, 0) : njs_min(end, length);

    value = njs_arg(args, nargs, 1);

    if (array != NULL) {
        for (i = start; i < end; i++) {
            array->start[i] = *value;
        }

        vm->retval = *this;

        return NJS_OK;
    }

    value = njs_arg(args, nargs, 1);

    while (start < end) {
        ret = njs_value_property_i64_set(vm, this, start++, value);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

    vm->retval = *this;

    return NJS_OK;
}