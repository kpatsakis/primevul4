njs_array_handler_filter(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    njs_int_t    ret;
    njs_value_t  copy;

    if (njs_is_valid(entry)) {
        copy = *entry;

        ret = njs_array_iterator_call(vm, args, &copy, n);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }

        if (njs_is_true(&vm->retval)) {
            ret = njs_array_add(vm, args->data, &copy);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }
        }
    }

    return NJS_OK;
}