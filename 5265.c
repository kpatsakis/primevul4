njs_function_name_set(njs_vm_t *vm, njs_function_t *function,
    njs_value_t *name, const char *prefix)
{
    u_char              *p;
    size_t              len, symbol;
    njs_int_t           ret;
    njs_value_t         value;
    njs_string_prop_t   string;
    njs_object_prop_t   *prop;
    njs_lvlhsh_query_t  lhq;

    prop = njs_object_prop_alloc(vm, &njs_string_name, name, 0);
    if (njs_slow_path(prop == NULL)) {
        return NJS_ERROR;
    }

    symbol = 0;

    if (njs_is_symbol(&prop->value)) {
        symbol = 2;
        prop->value = *njs_symbol_description(&prop->value);
    }

    if (prefix != NULL || symbol != 0) {
        value = prop->value;
        (void) njs_string_prop(&string, &value);

        len = (prefix != NULL) ? njs_strlen(prefix) + 1: 0;
        p = njs_string_alloc(vm, &prop->value, string.size + len + symbol,
                             string.length + len + symbol);
        if (njs_slow_path(p == NULL)) {
            return NJS_ERROR;
        }

        if (len != 0) {
            p = njs_cpymem(p, prefix, len - 1);
            *p++ = ' ';
        }

        if (symbol != 0) {
            *p++ = '[';
        }

        p = njs_cpymem(p, string.start, string.size);

        if (symbol != 0) {
            *p++ = ']';
        }
    }

    prop->configurable = 1;

    lhq.key_hash = NJS_NAME_HASH;
    lhq.key = njs_str_value("name");
    lhq.replace = 0;
    lhq.value = prop;
    lhq.proto = &njs_object_hash_proto;
    lhq.pool = vm->mem_pool;

    ret = njs_lvlhsh_insert(&function->object.hash, &lhq);
    if (njs_slow_path(ret != NJS_OK)) {
        njs_internal_error(vm, "lvlhsh insert failed");
        return NJS_ERROR;
    }

    return NJS_OK;
}