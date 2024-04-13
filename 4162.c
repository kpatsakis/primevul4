njs_array_handler_some(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    njs_int_t  ret;

    if (njs_is_valid(entry)) {
        ret = njs_array_iterator_call(vm, args, entry, n);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }

        if (njs_is_true(&vm->retval)) {
            vm->retval = njs_value_true;
            return NJS_DONE;
        }
    }

    return NJS_OK;
}