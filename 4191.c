njs_array_prototype_reverse_iterator(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t type)
{
    int64_t                 from, length;
    njs_int_t               ret;
    njs_value_t             accumulator;
    njs_iterator_args_t     iargs;
    njs_iterator_handler_t  handler;

    iargs.value = njs_argument(args, 0);

    ret = njs_value_to_object(vm, iargs.value);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    iargs.argument = njs_arg(args, nargs, 1);

    ret = njs_value_length(vm, iargs.value, &length);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    switch (type) {
    case NJS_ARRAY_LAST_INDEX_OF:
        handler = njs_array_handler_index_of;

        if (length == 0) {
            goto done;
        }

        if (nargs > 2) {
            ret = njs_value_to_integer(vm, njs_arg(args, nargs, 2), &from);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }

        } else {
            from = length - 1;
        }

        if (from >= 0) {
            from = njs_min(from, length - 1);

        } else if (from < 0) {
            from += length;
        }

        break;

    case NJS_ARRAY_REDUCE_RIGHT:
    default:
        handler = njs_array_handler_reduce;

        if (njs_slow_path(!njs_is_function(njs_arg(args, nargs, 1)))) {
            njs_type_error(vm, "callback argument is not callable");
            return NJS_ERROR;
        }

        njs_set_invalid(&accumulator);

        iargs.function = njs_function(njs_argument(args, 1));
        iargs.argument = &accumulator;

        if (nargs > 2) {
            accumulator = *njs_argument(args, 2);

        } else if (length == 0) {
            goto done;
        }

        from = length - 1;
        break;
    }

    iargs.from = from;
    iargs.to = 0;

    ret = njs_object_iterate_reverse(vm, &iargs, handler);
    if (njs_fast_path(ret == NJS_ERROR)) {
        return NJS_ERROR;
    }

    if (ret == NJS_DONE) {
        return NJS_OK;
    }

done:

    switch (type) {
    case NJS_ARRAY_LAST_INDEX_OF:
        njs_set_number(&vm->retval, -1);
        break;

    case NJS_ARRAY_REDUCE_RIGHT:
    default:
        if (!njs_is_valid(&accumulator)) {
            njs_type_error(vm, "Reduce of empty object with no initial value");
            return NJS_ERROR;
        }

        vm->retval = accumulator;
        break;
    }

    return NJS_OK;
}