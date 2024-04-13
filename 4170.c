njs_array_prototype_reverse(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t      length, l, h;
    njs_int_t    ret, lret, hret;
    njs_value_t  value, lvalue, hvalue, *this;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_object_length(vm, this, &length);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (njs_slow_path(length < 2)) {
        vm->retval = *this;
        return NJS_OK;
    }

    for (l = 0, h = length - 1; l < h; l++, h--) {
        lret = njs_value_property_i64(vm, this, l, &lvalue);
        if (njs_slow_path(lret == NJS_ERROR)) {
            return NJS_ERROR;
        }

        hret = njs_value_property_i64(vm, this, h, &hvalue);
        if (njs_slow_path(hret == NJS_ERROR)) {
            return NJS_ERROR;
        }

        if (lret == NJS_OK) {
            ret = njs_value_property_i64_set(vm, this, h, &lvalue);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return NJS_ERROR;
            }

            if (hret == NJS_OK) {
                ret = njs_value_property_i64_set(vm, this, l, &hvalue);
                if (njs_slow_path(ret == NJS_ERROR)) {
                    return NJS_ERROR;
                }

            } else {
                ret = njs_value_property_i64_delete(vm, this, l, &value);
                if (njs_slow_path(ret == NJS_ERROR)) {
                    return NJS_ERROR;
                }
            }

        } else if (hret == NJS_OK) {
            ret = njs_value_property_i64_set(vm, this, l, &hvalue);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return NJS_ERROR;
            }

            ret = njs_value_property_i64_delete(vm, this, h, &value);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return NJS_ERROR;
            }
        }
    }

    vm->retval = *this;

    return NJS_OK;
}