njs_array_length_set(njs_vm_t *vm, njs_value_t *value,
    njs_object_prop_t *prev, njs_value_t *setval)
{
    double        num, idx;
    int64_t       prev_length;
    uint32_t      i, length;
    njs_int_t     ret;
    njs_array_t   *array, *keys;

    array = njs_object_proto_lookup(njs_object(value), NJS_ARRAY, njs_array_t);
    if (njs_slow_path(array == NULL)) {
        return NJS_DECLINED;
    }

    ret = njs_value_to_number(vm, setval, &num);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    length = (uint32_t) njs_number_to_length(num);
    if ((double) length != num) {
        njs_range_error(vm, "Invalid array length");
        return NJS_ERROR;
    }

    ret = njs_value_to_length(vm, &prev->value, &prev_length);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    keys = NULL;

    if (length < prev_length) {
        keys = njs_array_indices(vm, value);
        if (njs_slow_path(keys == NULL)) {
            return NJS_ERROR;
        }

        if (keys->length != 0) {
            i = keys->length - 1;

            do {
                idx = njs_string_to_index(&keys->start[i]);
                if (idx >= length) {
                    ret = njs_value_property_delete(vm, value, &keys->start[i],
                                                    NULL);
                    if (njs_slow_path(ret == NJS_ERROR)) {
                        goto done;
                    }
                }
            } while (i-- != 0);
        }
    }

    ret = njs_array_length_redefine(vm, value, length);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = NJS_OK;

done:

    if (keys != NULL) {
        njs_array_destroy(vm, keys);
    }

    return ret;
}