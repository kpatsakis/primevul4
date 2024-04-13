njs_array_iterator_call(njs_vm_t *vm, njs_iterator_args_t *args,
    const njs_value_t *entry, uint32_t n)
{
    njs_value_t  arguments[3];

    /* GC: array elt, array */

    arguments[0] = *entry;
    njs_set_number(&arguments[1], n);
    arguments[2] = *args->value;

    return njs_function_call(vm, args->function, args->argument, arguments, 3,
                             &vm->retval);
}