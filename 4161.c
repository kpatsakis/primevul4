njs_array_length(njs_vm_t *vm,njs_object_prop_t *prop, njs_value_t *value,
    njs_value_t *setval, njs_value_t *retval)
{
    double        num;
    int64_t       size;
    uint32_t      length;
    njs_int_t     ret;
    njs_value_t   *val;
    njs_array_t   *array;
    njs_object_t  *proto;

    proto = njs_object(value);

    if (njs_fast_path(setval == NULL)) {
        array = njs_object_proto_lookup(proto, NJS_ARRAY, njs_array_t);
        if (njs_slow_path(array == NULL)) {
            njs_set_undefined(retval);
            return NJS_DECLINED;
        }

        njs_set_number(retval, array->length);
        return NJS_OK;
    }

    if (proto->type != NJS_ARRAY) {
        njs_set_undefined(retval);
        return NJS_DECLINED;
    }

    if (njs_slow_path(!njs_is_valid(setval))) {
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

    array = (njs_array_t *) proto;

    if (njs_fast_path(array->object.fast_array)) {
        if (njs_fast_path(length <= NJS_ARRAY_LARGE_OBJECT_LENGTH)) {
            size = (int64_t) length - array->length;

            if (size > 0) {
                ret = njs_array_expand(vm, array, 0, size);
                if (njs_slow_path(ret != NJS_OK)) {
                    return NJS_ERROR;
                }

                val = &array->start[array->length];

                do {
                    njs_set_invalid(val);
                    val++;
                    size--;
                } while (size != 0);
            }

            array->length = length;

            *retval = *setval;
            return NJS_OK;
        }

        ret = njs_array_convert_to_slow_array(vm, array);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }
    }

    prop->type = NJS_PROPERTY;
    njs_set_number(&prop->value, length);

    *retval = *setval;

    return NJS_OK;
}