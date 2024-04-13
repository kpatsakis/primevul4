njs_function_arguments_object_init(njs_vm_t *vm, njs_native_frame_t *frame)
{
    njs_int_t           ret;
    njs_uint_t          nargs, n;
    njs_value_t         value;
    njs_object_t        *arguments;
    njs_object_prop_t   *prop;
    njs_lvlhsh_query_t  lhq;

    static const njs_value_t  njs_string_length = njs_string("length");

    arguments = njs_object_alloc(vm);
    if (njs_slow_path(arguments == NULL)) {
        return NJS_ERROR;
    }

    arguments->shared_hash = vm->shared->arguments_object_instance_hash;

    nargs = frame->nargs;

    njs_set_number(&value, nargs);

    prop = njs_object_prop_alloc(vm, &njs_string_length, &value, 1);
    if (njs_slow_path(prop == NULL)) {
        return NJS_ERROR;
    }

    prop->enumerable = 0;

    lhq.value = prop;
    lhq.key_hash = NJS_LENGTH_HASH;
    njs_string_get(&prop->name, &lhq.key);

    lhq.replace = 0;
    lhq.pool = vm->mem_pool;
    lhq.proto = &njs_object_hash_proto;

    ret = njs_lvlhsh_insert(&arguments->hash, &lhq);
    if (njs_slow_path(ret != NJS_OK)) {
        njs_internal_error(vm, "lvlhsh insert failed");
        return NJS_ERROR;
    }

    for (n = 0; n < nargs; n++) {
        njs_uint32_to_string(&value, n);

        prop = njs_object_prop_alloc(vm, &value, &frame->arguments[n], 1);
        if (njs_slow_path(prop == NULL)) {
            return NJS_ERROR;
        }

        lhq.value = prop;
        njs_string_get(&prop->name, &lhq.key);
        lhq.key_hash = njs_djb_hash(lhq.key.start, lhq.key.length);

        ret = njs_lvlhsh_insert(&arguments->hash, &lhq);
        if (njs_slow_path(ret != NJS_OK)) {
            njs_internal_error(vm, "lvlhsh insert failed");
            return NJS_ERROR;
        }
    }

    frame->arguments_object = arguments;

    return NJS_OK;
}