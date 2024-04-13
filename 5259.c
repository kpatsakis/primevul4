njs_function_instance_length(njs_vm_t *vm, njs_object_prop_t *prop,
    njs_value_t *value, njs_value_t *setval, njs_value_t *retval)
{
    njs_object_t    *proto;
    njs_function_t  *function;

    proto = njs_object(value);

    do {
        if (njs_fast_path(proto->type == NJS_FUNCTION)) {
            break;
        }

        proto = proto->__proto__;
    } while (proto != NULL);

    if (njs_slow_path(proto == NULL)) {
        njs_internal_error(vm, "no function in proto chain");
        return NJS_ERROR;
    }

    function = (njs_function_t *) proto;

    njs_set_number(retval, function->args_count);

    return NJS_OK;
}