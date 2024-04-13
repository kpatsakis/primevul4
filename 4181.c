njs_array_handler_includes(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    if (!njs_is_valid(entry)) {
        entry = njs_value_arg(&njs_value_undefined);
    }

    if (njs_values_same_zero(args->argument, entry)) {
        njs_set_true(&vm->retval);

        return NJS_DONE;
    }

    return NJS_OK;
}