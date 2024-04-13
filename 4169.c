njs_array_is_array(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t unused)
{
    const njs_value_t  *value;

    if (nargs > 1 && njs_is_array(&args[1])) {
        value = &njs_value_true;

    } else {
        value = &njs_value_false;
    }

    vm->retval = *value;

    return NJS_OK;
}