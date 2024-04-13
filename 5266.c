njs_function_copy(njs_vm_t *vm, njs_function_t *function)
{
    size_t             size, n;
    njs_value_t        **from, **to;
    njs_function_t     *copy;
    njs_object_type_t  type;

    n = (function->native) ? 0 : function->u.lambda->nclosures;

    size = sizeof(njs_function_t) + n * sizeof(njs_value_t *);

    copy = njs_mp_alloc(vm->mem_pool, size);
    if (njs_slow_path(copy == NULL)) {
        return NULL;
    }

    *copy = *function;

    type = njs_function_object_type(vm, function);

    copy->object.__proto__ = &vm->prototypes[type].object;
    copy->object.shared = 0;

    if (n == 0) {
        return copy;
    }

    from = njs_function_closures(function);
    to = njs_function_closures(copy);

    do {
        n--;

        to[n] = from[n];

    } while (n != 0);

    return copy;
}