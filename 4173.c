njs_array_prototype_shift(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t      i, length;
    njs_int_t    ret;
    njs_array_t  *array;
    njs_value_t  *this, entry;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_object_length(vm, this, &length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (length == 0) {
        ret = njs_object_length_set(vm, this, length);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        njs_set_undefined(&vm->retval);

        return NJS_OK;
    }

    ret = njs_value_property_i64(vm, this, 0, &vm->retval);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return NJS_ERROR;
    }

    if (njs_is_fast_array(this)) {
        array = njs_array(this);

        array->start++;
        array->length--;

    } else {

        ret = njs_value_property_i64_delete(vm, this, 0, &vm->retval);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        for (i = 1; i < length; i++) {
            ret = njs_value_property_i64_delete(vm, this, i, &entry);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return ret;
            }

            if (ret == NJS_OK) {
                ret = njs_value_property_i64_set(vm, this, i - 1, &entry);
                if (njs_slow_path(ret == NJS_ERROR)) {
                    return ret;
                }
            }
        }

        ret = njs_object_length_set(vm, this, length - 1);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

    return NJS_OK;
}