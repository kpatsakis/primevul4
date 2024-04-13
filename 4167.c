njs_array_handler_index_of(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    if (njs_values_strict_equal(args->argument, entry)) {
        njs_set_number(&vm->retval, n);

        return NJS_DONE;
    }

    return NJS_OK;
}