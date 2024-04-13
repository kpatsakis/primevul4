njs_function_capture_global_closures(njs_vm_t *vm, njs_function_t *function)
{
    void                   *start, *end;
    uint32_t               n;
    njs_value_t            *value, **refs, **global;
    njs_index_t            *indexes, index;
    njs_native_frame_t     *native;
    njs_function_lambda_t  *lambda;

    lambda = function->u.lambda;

    if (lambda->nclosures == 0) {
        return NJS_OK;
    }

    native = vm->top_frame;

    while (native->previous->function != NULL) {
        native = native->previous;
    }

    start = native;
    end = native->free;

    indexes = lambda->closures;
    refs = njs_function_closures(function);

    global = vm->levels[NJS_LEVEL_GLOBAL];

    n = lambda->nclosures;

    while (n > 0) {
        n--;

        index = indexes[n];

        switch (njs_scope_index_type(index)) {
        case NJS_LEVEL_LOCAL:
            value = njs_function_closure_value(vm, native->local, index,
                                               start, end);
            break;

        case NJS_LEVEL_GLOBAL:
            value = njs_function_closure_value(vm, global, index, start, end);
            break;

        default:
            njs_type_error(vm, "unexpected value type for closure \"%uD\"",
                           njs_scope_index_type(index));
            return NJS_ERROR;
        }

        if (njs_slow_path(value == NULL)) {
            return NJS_ERROR;
        }

        refs[n] = value;
    }

    function->closure_copied = 1;

    return NJS_OK;
}