njs_array_prototype_iterator(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t magic)
{
    int64_t                 i, length;
    njs_int_t               ret;
    njs_array_t             *array;
    njs_value_t             accumulator;
    njs_iterator_args_t     iargs;
    njs_iterator_handler_t  handler;

    iargs.value = njs_argument(args, 0);

    ret = njs_value_to_object(vm, iargs.value);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_value_length(vm, iargs.value, &iargs.to);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    iargs.from = 0;

    if (njs_array_arg1(magic) == NJS_ARRAY_FUNC) {
        if (njs_slow_path(!njs_is_function(njs_arg(args, nargs, 1)))) {
            njs_type_error(vm, "callback argument is not callable");
            return NJS_ERROR;
        }

        iargs.function = njs_function(njs_argument(args, 1));
        iargs.argument = njs_arg(args, nargs, 2);

    } else {
        iargs.argument = njs_arg(args, nargs, 1);
    }

    switch (njs_array_type(magic)) {
    case NJS_ARRAY_EVERY:
        handler = njs_array_handler_every;
        break;

    case NJS_ARRAY_SOME:
        handler = njs_array_handler_some;
        break;

    case NJS_ARRAY_INCLUDES:
    case NJS_ARRAY_INDEX_OF:
        switch (njs_array_type(magic)) {
        case NJS_ARRAY_INCLUDES:
            handler = njs_array_handler_includes;

            if (iargs.to == 0) {
                goto done;
            }

            break;

        default:
            handler = njs_array_handler_index_of;
        }

        ret = njs_value_to_integer(vm, njs_arg(args, nargs, 2), &iargs.from);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }

        if (iargs.from < 0) {
            iargs.from += iargs.to;

            if (iargs.from < 0) {
                iargs.from = 0;
            }
        }

        break;

    case NJS_ARRAY_FOR_EACH:
        handler = njs_array_handler_for_each;
        break;

    case NJS_ARRAY_FIND:
        handler = njs_array_handler_find;
        break;

    case NJS_ARRAY_FIND_INDEX:
        handler = njs_array_handler_find_index;
        break;

    case NJS_ARRAY_REDUCE:
        handler = njs_array_handler_reduce;

        njs_set_invalid(&accumulator);

        if (nargs > 2) {
            accumulator = *iargs.argument;
        }

        iargs.argument = &accumulator;
        break;

    case NJS_ARRAY_FILTER:
    case NJS_ARRAY_MAP:
    default:
        if (njs_array_type(magic) == NJS_ARRAY_FILTER) {
            length = 0;
            handler = njs_array_handler_filter;

        } else {
            length = iargs.to;
            handler = njs_array_handler_map;
        }

        array = njs_array_alloc(vm, 0, length, NJS_ARRAY_SPARE);
        if (njs_slow_path(array == NULL)) {
            return NJS_ERROR;
        }

        if (array->object.fast_array) {
            for (i = 0; i < length; i++) {
                njs_set_invalid(&array->start[i]);
            }
        }

        iargs.data = array;

        break;
    }

    ret = njs_object_iterate(vm, &iargs, handler);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (ret == NJS_DONE) {
        return NJS_OK;
    }

done:

    /* Default values. */

    switch (njs_array_type(magic)) {
    case NJS_ARRAY_EVERY:
        vm->retval = njs_value_true;
        break;

    case NJS_ARRAY_SOME:
    case NJS_ARRAY_INCLUDES:
        vm->retval = njs_value_false;
        break;

    case NJS_ARRAY_INDEX_OF:
    case NJS_ARRAY_FIND_INDEX:
        njs_set_number(&vm->retval, -1);
        break;

    case NJS_ARRAY_FOR_EACH:
    case NJS_ARRAY_FIND:
        njs_set_undefined(&vm->retval);
        break;

    case NJS_ARRAY_REDUCE:
        if (!njs_is_valid(&accumulator)) {
            njs_type_error(vm, "Reduce of empty object with no initial value");
            return NJS_ERROR;
        }

        vm->retval = accumulator;
        break;

    case NJS_ARRAY_FILTER:
    case NJS_ARRAY_MAP:
    default:
        njs_set_array(&vm->retval, iargs.data);
    }

    return NJS_OK;
}