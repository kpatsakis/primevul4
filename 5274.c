njs_function_object_type(njs_vm_t *vm, njs_function_t *function)
{
    if (function->object.shared_hash.slot
        == vm->shared->async_function_instance_hash.slot)
    {
        return NJS_OBJ_TYPE_ASYNC_FUNCTION;
    }

    return NJS_OBJ_TYPE_FUNCTION;
}