njs_function_alloc(njs_vm_t *vm, njs_function_lambda_t *lambda,
    njs_bool_t async)
{
    size_t          size;
    njs_object_t    *proto;
    njs_function_t  *function;

    size = sizeof(njs_function_t) + lambda->nclosures * sizeof(njs_value_t *);

    function = njs_mp_zalloc(vm->mem_pool, size);
    if (njs_slow_path(function == NULL)) {
        goto fail;
    }

    /*
     * njs_mp_zalloc() does also:
     *   njs_lvlhsh_init(&function->object.hash);
     *   function->object.__proto__ = NULL;
     */

    function->ctor = lambda->ctor;
    function->args_offset = 1;
    function->u.lambda = lambda;

    if (function->ctor) {
        function->object.shared_hash = vm->shared->function_instance_hash;

    } else if (async) {
        function->object.shared_hash = vm->shared->async_function_instance_hash;

    } else {
        function->object.shared_hash = vm->shared->arrow_instance_hash;
    }

    if (async) {
        proto = &vm->prototypes[NJS_OBJ_TYPE_ASYNC_FUNCTION].object;

    } else {
        proto = &vm->prototypes[NJS_OBJ_TYPE_FUNCTION].object;
    }

    function->object.__proto__ = proto;
    function->object.type = NJS_FUNCTION;

    function->object.extensible = 1;

    return function;

fail:

    njs_memory_error(vm);

    return NULL;
}