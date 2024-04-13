njs_function_property_prototype_set(njs_vm_t *vm, njs_lvlhsh_t *hash,
    njs_value_t *prototype)
{
    njs_int_t           ret;
    njs_object_prop_t   *prop;
    njs_lvlhsh_query_t  lhq;

    const njs_value_t  proto_string = njs_string("prototype");

    prop = njs_object_prop_alloc(vm, &proto_string, prototype, 0);
    if (njs_slow_path(prop == NULL)) {
        return NULL;
    }

    prop->writable = 1;

    lhq.value = prop;
    lhq.key_hash = NJS_PROTOTYPE_HASH;
    lhq.key = njs_str_value("prototype");
    lhq.replace = 1;
    lhq.pool = vm->mem_pool;
    lhq.proto = &njs_object_hash_proto;

    ret = njs_lvlhsh_insert(hash, &lhq);

    if (njs_fast_path(ret == NJS_OK)) {
        return &prop->value;
    }

    njs_internal_error(vm, "lvlhsh insert failed");

    return NULL;
}