njs_array_prototype_slice(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t      start, end, length, object_length;
    njs_int_t    ret;
    njs_value_t  *this;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_object_length(vm, this, &object_length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    length = object_length;

    ret = njs_value_to_integer(vm, njs_arg(args, nargs, 1), &start);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (start < 0) {
        start += length;

        if (start < 0) {
            start = 0;
        }
    }

    if (start >= length) {
        start = 0;
        length = 0;

    } else {
        if (njs_is_defined(njs_arg(args, nargs, 2))) {
            ret = njs_value_to_integer(vm, njs_argument(args, 2), &end);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }

        } else {
            end = length;
        }

        if (end < 0) {
            end += length;
        }

        if (length >= end) {
            length = end - start;

            if (length < 0) {
                start = 0;
                length = 0;
            }

        } else {
            length -= start;
        }
    }

    return njs_array_prototype_slice_copy(vm, this, start, length);
}