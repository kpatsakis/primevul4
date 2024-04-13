njs_function_value_copy(njs_vm_t *vm, njs_value_t *value)
{
    njs_function_t     *function, *copy;
    njs_object_type_t  type;

    function = njs_function(value);

    if (!function->object.shared) {
        return function;
    }

    copy = njs_function_copy(vm, function);
    if (njs_slow_path(copy == NULL)) {
        njs_memory_error(vm);
        return NULL;
    }

    type = njs_function_object_type(vm, function);

    if (copy->ctor) {
        copy->object.shared_hash = vm->shared->function_instance_hash;

    } else if (type == NJS_OBJ_TYPE_ASYNC_FUNCTION) {
        copy->object.shared_hash = vm->shared->async_function_instance_hash;

    } else {
        copy->object.shared_hash = vm->shared->arrow_instance_hash;
    }

    value->data.u.function = copy;

    return copy;
}