njs_array_copy_within(njs_vm_t *vm, njs_value_t *array, int64_t to_pos,
    int64_t from_pos, int64_t count, njs_bool_t forward)
{
    int64_t      i, from, to;
    njs_int_t    ret;
    njs_array_t  *arr;
    njs_value_t  value;

    if (njs_fast_path(njs_is_fast_array(array) && count > 0)) {
        arr = njs_array(array);

        memmove(&arr->start[to_pos], &arr->start[from_pos],
                count * sizeof(njs_value_t));

        return NJS_OK;
    }

    if (!forward) {
        from_pos += count - 1;
        to_pos += count - 1;
    }

    for (i = 0; i < count; i++) {
        if (forward) {
            from = from_pos + i;
            to = to_pos + i;

        } else {
            from = from_pos - i;
            to = to_pos - i;
        }

        ret = njs_value_property_i64(vm, array, from, &value);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return NJS_ERROR;
        }

        if (ret == NJS_OK) {
            ret = njs_value_property_i64_set(vm, array, to, &value);

        } else {
            ret = njs_value_property_i64_delete(vm, array, to, NULL);
        }

        if (njs_slow_path(ret == NJS_ERROR)) {
            return NJS_ERROR;
        }
    }

    return NJS_OK;
}