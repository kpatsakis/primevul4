njs_function_prototype_bind(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    size_t              size;
    njs_int_t           ret;
    njs_value_t         *values, name;
    njs_function_t      *function;
    njs_lvlhsh_query_t  lhq;

    if (!njs_is_function(&args[0])) {
        njs_type_error(vm, "\"this\" argument is not a function");
        return NJS_ERROR;
    }

    function = njs_mp_alloc(vm->mem_pool, sizeof(njs_function_t));
    if (njs_slow_path(function == NULL)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    *function = *njs_function(&args[0]);

    njs_lvlhsh_init(&function->object.hash);

    /* Bound functions have no "prototype" property. */
    function->object.shared_hash = vm->shared->arrow_instance_hash;

    function->object.__proto__ = &vm->prototypes[NJS_OBJ_TYPE_FUNCTION].object;
    function->object.shared = 0;

    function->u.bound_target = njs_function(&args[0]);

    njs_object_property_init(&lhq, &njs_string_name, NJS_NAME_HASH);

    ret = njs_object_property(vm, &args[0], &lhq, &name);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (!njs_is_string(&name)) {
        name = njs_string_empty;
    }

    ret = njs_function_name_set(vm, function, &name, "bound");
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (nargs == 1) {
        args = njs_value_arg(&njs_value_undefined);

    } else {
        nargs--;
        args++;
    }

    if (nargs > function->args_count) {
        function->args_count = 0;

    } else {
        function->args_count -= nargs - 1;
    }

    function->args_offset = nargs;
    size = nargs * sizeof(njs_value_t);

    values = njs_mp_alloc(vm->mem_pool, size);
    if (njs_slow_path(values == NULL)) {
        njs_memory_error(vm);
        njs_mp_free(vm->mem_pool, function);
        return NJS_ERROR;
    }

    function->bound = values;

    /* GC: ? retain args. */

    memcpy(values, args, size);

    njs_set_function(&vm->retval, function);

    return NJS_OK;
}