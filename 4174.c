njs_array_handler_map(njs_vm_t *vm, njs_iterator_args_t *args,
    njs_value_t *entry, int64_t n)
{
    njs_int_t    ret;
    njs_array_t  *retval;
    njs_value_t  this;

    retval = args->data;

    if (retval->object.fast_array) {
        njs_set_invalid(&retval->start[n]);
    }

    if (njs_is_valid(entry)) {
        ret = njs_array_iterator_call(vm, args, entry, n);
        if (njs_slow_path(ret != NJS_OK)) {
            return ret;
        }

        if (njs_is_valid(&vm->retval)) {
            if (retval->object.fast_array) {
                retval->start[n] = vm->retval;

            } else {
                njs_set_array(&this, retval);

                ret = njs_value_property_i64_set(vm, &this, n, &vm->retval);
                if (njs_slow_path(ret != NJS_OK)) {
                    return ret;
                }
            }
        }
    }

    return NJS_OK;
}