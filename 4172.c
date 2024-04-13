njs_array_prototype_join(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    u_char             *p, *last;
    int64_t            i, size, len, length;
    njs_int_t          ret;
    njs_chb_t          chain;
    njs_utf8_t         utf8;
    njs_value_t        *value, *this, entry;
    njs_string_prop_t  separator, string;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    value = njs_arg(args, nargs, 1);

    if (njs_slow_path(!njs_is_string(value))) {
        if (njs_is_undefined(value)) {
            value = njs_value_arg(&njs_string_comma);

        } else {
            ret = njs_value_to_string(vm, value, value);
            if (njs_slow_path(ret != NJS_OK)) {
                return ret;
            }
        }
    }

    (void) njs_string_prop(&separator, value);

    if (njs_slow_path(!njs_is_object(this))) {
        vm->retval = njs_string_empty;
        return NJS_OK;
    }

    length = 0;
    utf8 = njs_is_byte_string(&separator) ? NJS_STRING_BYTE : NJS_STRING_UTF8;

    ret = njs_object_length(vm, this, &len);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (njs_slow_path(len == 0)) {
        vm->retval = njs_string_empty;
        return NJS_OK;
    }

    value = &entry;

    njs_chb_init(&chain, vm->mem_pool);

    for (i = 0; i < len; i++) {
        ret = njs_value_property_i64(vm, this, i, value);
        if (njs_slow_path(ret == NJS_ERROR)) {
            return ret;
        }

        if (!njs_is_null_or_undefined(value)) {
            if (!njs_is_string(value)) {
                last = njs_chb_current(&chain);

                ret = njs_value_to_chain(vm, &chain, value);
                if (njs_slow_path(ret < NJS_OK)) {
                    return ret;
                }

                if (last != njs_chb_current(&chain) && ret == 0) {
                    /*
                     * Appended values was a byte string.
                     */
                    utf8 = NJS_STRING_BYTE;
                }

                length += ret;

            } else {
                (void) njs_string_prop(&string, value);

                if (njs_is_byte_string(&string)) {
                    utf8 = NJS_STRING_BYTE;
                }

                length += string.length;
                njs_chb_append(&chain, string.start, string.size);
            }
        }

        length += separator.length;
        njs_chb_append(&chain, separator.start, separator.size);

        if (njs_slow_path(length > NJS_STRING_MAX_LENGTH)) {
            njs_range_error(vm, "invalid string length");
            return NJS_ERROR;
        }
    }

    njs_chb_drop(&chain, separator.size);

    size = njs_chb_size(&chain);
    if (njs_slow_path(size < 0)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    length -= separator.length;

    p = njs_string_alloc(vm, &vm->retval, size, utf8 ? length : 0);
    if (njs_slow_path(p == NULL)) {
        return NJS_ERROR;
    }

    njs_chb_join_to(&chain, p);
    njs_chb_destroy(&chain);

    return NJS_OK;
}