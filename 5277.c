njs_function_prototype_create(njs_vm_t *vm, njs_object_prop_t *prop,
    njs_value_t *value, njs_value_t *setval, njs_value_t *retval)
{
    njs_value_t     *proto, proto_value, *cons;
    njs_object_t    *prototype;
    njs_function_t  *function;

    if (setval == NULL) {
        prototype = njs_object_alloc(vm);
        if (njs_slow_path(prototype == NULL)) {
            return NJS_ERROR;
        }

        njs_set_object(&proto_value, prototype);

        setval = &proto_value;
    }

    function = njs_function_value_copy(vm, value);
    if (njs_slow_path(function == NULL)) {
        return NJS_ERROR;
    }

    proto = njs_function_property_prototype_set(vm, njs_object_hash(value),
                                                setval);
    if (njs_slow_path(proto == NULL)) {
        return NJS_ERROR;
    }

    if (setval == &proto_value && njs_is_object(proto)) {
        /* Only in getter context. */
        cons = njs_property_constructor_set(vm, njs_object_hash(proto), value);
        if (njs_slow_path(cons == NULL)) {
            return NJS_ERROR;
        }
    }

    *retval = *proto;

    return NJS_OK;
}