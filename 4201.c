njs_array_prototype_to_string(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    njs_int_t           ret;
    njs_value_t         value;
    njs_lvlhsh_query_t  lhq;

    static const njs_value_t  join_string = njs_string("join");

    if (njs_is_object(&args[0])) {
        njs_object_property_init(&lhq, &join_string, NJS_JOIN_HASH);

        ret = njs_object_property(vm, &args[0], &lhq, &value);

        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        if (njs_is_function(&value)) {
            return njs_function_apply(vm, njs_function(&value), args, nargs,
                                      &vm->retval);
        }
    }

    return njs_object_prototype_to_string(vm, args, nargs, unused);
}