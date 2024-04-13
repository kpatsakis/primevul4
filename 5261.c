njs_function_capture_closure(njs_vm_t *vm, njs_function_t *function,
    njs_function_lambda_t *lambda)
{
    void                *start, *end;
    uint32_t            n;
    njs_value_t         *value, **closure;
    njs_native_frame_t  *frame;

    if (lambda->nclosures == 0) {
        return NJS_OK;
    }

    frame = &vm->active_frame->native;

    while (frame->native) {
        frame = frame->previous;
    }

    start = frame;
    end = frame->free;

    closure = njs_function_closures(function);
    n = lambda->nclosures;

    do {
        n--;

        value = njs_scope_value(vm, lambda->closures[n]);

        if (start <= (void *) value && (void *) value < end) {
            value = njs_scope_value_clone(vm, lambda->closures[n], value);
            if (njs_slow_path(value == NULL)) {
                return NJS_ERROR;
            }
        }

        closure[n] = value;

    } while (n != 0);

    return NJS_OK;
}