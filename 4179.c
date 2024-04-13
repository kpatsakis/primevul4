njs_array_prototype_slice_copy(njs_vm_t *vm, njs_value_t *this,
    int64_t start, int64_t length)
{
    size_t             size;
    u_char             *dst;
    uint32_t           n;
    njs_int_t          ret;
    njs_array_t        *array, *keys;
    njs_value_t        *value, *last, retval, self;
    const u_char       *src, *end;
    njs_slice_prop_t   string_slice;
    njs_string_prop_t  string;

    keys = NULL;
    array = njs_array_alloc(vm, 0, length, NJS_ARRAY_SPARE);
    if (njs_slow_path(array == NULL)) {
        return NJS_ERROR;
    }

    if (njs_slow_path(length == 0)) {
        ret = NJS_OK;
        goto done;
    }

    n = 0;

    if (njs_fast_path(array->object.fast_array)) {
        if (njs_is_string(this) || njs_is_object_string(this)) {

            if (njs_is_object_string(this)) {
                this = njs_object_value(this);
            }

            string_slice.start = start;
            string_slice.length = length;
            string_slice.string_length = njs_string_prop(&string, this);

            njs_string_slice_string_prop(&string, &string, &string_slice);

            src = string.start;
            end = src + string.size;

            if (string.length == 0) {
                /* Byte string. */
                do {
                    value = &array->start[n++];
                    dst = njs_string_short_start(value);
                    *dst = *src++;
                    njs_string_short_set(value, 1, 0);

                    length--;
                } while (length != 0);

            } else {
                /* UTF-8 or ASCII string. */
                do {
                    value = &array->start[n++];
                    dst = njs_string_short_start(value);
                    dst = njs_utf8_copy(dst, &src, end);
                    size = dst - njs_string_short_start(value);
                    njs_string_short_set(value, size, 1);

                    length--;
                } while (length != 0);
            }

        } else if (njs_is_object(this)) {

            last = &array->start[length];

            for (value = array->start; value < last; value++, start++) {
                ret = njs_value_property_i64(vm, this, start, value);
                if (njs_slow_path(ret != NJS_OK)) {
                    if (ret == NJS_ERROR) {
                        return NJS_ERROR;
                    }

                    njs_set_invalid(value);
                }
            }

        } else {

            /* Primitive types. */

            value = array->start;

            do {
                njs_set_invalid(value++);
                length--;
            } while (length != 0);
        }

        ret = NJS_OK;
        goto done;
    }

    njs_set_array(&self, array);

    if (njs_fast_object(length)) {
        do {
            ret = njs_value_property_i64(vm, this, start++, &retval);
            if (njs_slow_path(ret == NJS_ERROR)) {
                return NJS_ERROR;
            }

            if (ret == NJS_OK) {
                ret = njs_value_property_i64_set(vm, &self, start, &retval);
                if (njs_slow_path(ret == NJS_ERROR)) {
                    return ret;
                }
            }

            length--;
        } while (length != 0);

        ret = NJS_OK;
        goto done;
    }

    keys = njs_array_indices(vm, this);
    if (njs_slow_path(keys == NULL)) {
        return NJS_ERROR;
    }

    for (n = 0; n < keys->length; n++) {
        ret = njs_value_property(vm, this, &keys->start[n], &retval);
        if (njs_slow_path(ret == NJS_ERROR)) {
            goto done;
        }

        ret = njs_value_property_set(vm, &self, &keys->start[n], &retval);
        if (njs_slow_path(ret == NJS_ERROR)) {
            goto done;
        }
    }

    ret = NJS_OK;

done:

    if (keys != NULL) {
        njs_array_destroy(vm, keys);
    }

    njs_set_array(&vm->retval, array);

    return ret;
}