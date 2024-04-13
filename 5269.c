njs_vm_function_alloc(njs_vm_t *vm, njs_function_native_t native)
{
    njs_function_t  *function;

    function = njs_mp_zalloc(vm->mem_pool, sizeof(njs_function_t));
    if (njs_slow_path(function == NULL)) {
        njs_memory_error(vm);
        return NULL;
    }

    function->native = 1;
    function->args_offset = 1;
    function->u.native = native;

    return function;
}