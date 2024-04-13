njs_array_handler_reduce(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    njs_int_t    ret;
    njs_value_t  arguments[5];

    if (njs_is_valid(entry)) {
        if (!njs_is_valid(args->argument)) {
            *(args->argument) = *entry;
            return NJS_OK;
        }

        /* GC: array elt, array */

        njs_set_undefined(&arguments[0]);
        arguments[1] = *args->argument;
        arguments[2] = *entry;
        njs_set_number(&arguments[3], n);
        arguments[4] = *args->value;

        ret =  njs_function_apply(vm, args->function, arguments, 5,
                                  args->argument);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    return NJS_OK;
}