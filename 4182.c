njs_array_prototype_copy_within(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t unused)
{
    int64_t      length, count, to, from, end;
    njs_int_t    ret;
    njs_value_t  *this, *value;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_value_length(vm, this, &length);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_value_to_integer(vm, njs_arg(args, nargs, 1), &to);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    to = (to < 0) ? njs_max(length + to, 0) : njs_min(to, length);

    ret = njs_value_to_integer(vm, njs_arg(args, nargs, 2), &from);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    from = (from < 0) ? njs_max(length + from, 0) : njs_min(from, length);

    value = njs_arg(args, nargs, 3);

    if (njs_is_undefined(value)) {
        end = length;

    } else {
        ret = njs_value_to_integer(vm, value, &end);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    end = (end < 0) ? njs_max(length + end, 0) : njs_min(end, length);

    count = njs_min(end - from, length - to);

    njs_vm_retval_set(vm, this);

    return njs_array_copy_within(vm, this, to, from, count,
                                 !(from < to && to < from + count));
}