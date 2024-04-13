njs_function_prototype_thrower(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t unused)
{
    njs_type_error(vm, "\"caller\", \"callee\", \"arguments\" "
                   "properties may not be accessed");
    return NJS_ERROR;
}