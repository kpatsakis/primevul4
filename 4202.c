njs_array_keys(njs_vm_t *vm, njs_value_t *object, njs_bool_t all)
{
    njs_array_t  *keys;

    keys = njs_value_own_enumerate(vm, object, NJS_ENUM_KEYS, NJS_ENUM_STRING,
                                   all);
    if (njs_slow_path(keys == NULL)) {
        return NULL;
    }

    njs_qsort(keys->start, keys->length, sizeof(njs_value_t),
              njs_array_indices_handler, NULL);

    return keys;
}