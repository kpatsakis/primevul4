njs_array_handler_for_each(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    if (njs_is_valid(entry)) {
        return njs_array_iterator_call(vm, args, entry, n);
    }

    return NJS_OK;
}