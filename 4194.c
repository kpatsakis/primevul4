njs_array_indices(njs_vm_t *vm, njs_value_t *object)
{
    double       idx;
    uint32_t     i;
    njs_array_t  *keys;

    keys = njs_array_keys(vm, object, 1);
    if (njs_slow_path(keys == NULL)) {
        return NULL;
    }

    for (i = 0; i < keys->length; i++) {
        idx = njs_string_to_index(&keys->start[i]);

        if (isnan(idx)) {
            keys->length = i;
            break;
        }
    }

    return keys;
}