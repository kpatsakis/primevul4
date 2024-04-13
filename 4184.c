njs_array_length_redefine(njs_vm_t *vm, njs_value_t *value, uint32_t length)
{
    njs_object_prop_t  *prop;

    static const njs_value_t  string_length = njs_string("length");

    if (njs_slow_path(!njs_is_array(value))) {
        njs_internal_error(vm, "njs_array_length_redefine() "
                           "applied to non-array");
        return NJS_ERROR;
    }

    prop = njs_object_property_add(vm, value, njs_value_arg(&string_length), 1);
    if (njs_slow_path(prop == NULL)) {
        njs_internal_error(vm, "njs_array_length_redefine() "
                           "cannot redefine \"length\"");
        return NJS_ERROR;
    }

    prop->enumerable = 0;
    prop->configurable = 0;

    njs_value_number_set(&prop->value, length);

    return NJS_OK;
}