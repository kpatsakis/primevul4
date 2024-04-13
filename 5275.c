njs_function_closure_value(njs_vm_t *vm, njs_value_t **scope, njs_index_t index,
    void *start, void *end)
{
    njs_value_t  *value, *newval;

    value = scope[njs_scope_index_value(index)];

    if (start <= (void *) value && end > (void *) value) {
        newval = njs_mp_alloc(vm->mem_pool, sizeof(njs_value_t));
        if (njs_slow_path(newval == NULL)) {
            njs_memory_error(vm);
            return NULL;
        }

        *newval = *value;
        value = newval;
    }

    scope[njs_scope_index_value(index)] = value;

    return value;
}