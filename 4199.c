njs_is_concat_spreadable(njs_vm_t *vm, njs_value_t *value)
{
    njs_int_t    ret;
    njs_value_t  retval;

    static const njs_value_t  key =
                         njs_wellknown_symbol(NJS_SYMBOL_IS_CONCAT_SPREADABLE);

    if (njs_slow_path(!njs_is_object(value))) {
        return NJS_DECLINED;
    }

    ret = njs_value_property(vm, value, njs_value_arg(&key), &retval);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return NJS_ERROR;
    }

    if (njs_is_defined(&retval)) {
        return njs_bool(&retval) ? NJS_OK : NJS_DECLINED;
    }

    return njs_is_array(value) ? NJS_OK : NJS_DECLINED;
}