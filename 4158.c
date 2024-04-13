njs_array_prototype_pop(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    int64_t      length;
    njs_int_t    ret;
    njs_value_t  *this;

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

    ret = njs_value_property_i64(vm, this, --length, &vm->retval);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (njs_is_fast_array(this)) {
        njs_array(this)->length--;

    } else {
        ret = njs_value_property_i64_delete(vm, this, length, NULL);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        ret = njs_object_length_set(vm, this, length);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }
    }

    return NJS_OK;
}