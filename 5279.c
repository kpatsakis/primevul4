njs_function_native_call(njs_vm_t *vm)
{
    njs_int_t              ret;
    njs_function_t         *function, *target;
    njs_native_frame_t     *native, *previous;
    njs_function_native_t  call;

    native = vm->top_frame;
    function = native->function;

    if (njs_fast_path(function->bound == NULL)) {
        call = function->u.native;

    } else {
        target = function->u.bound_target;

        if (njs_slow_path(target->bound != NULL)) {
            njs_internal_error(vm, "chain of bound function are not supported");
            return NJS_ERROR;
        }

        call = target->u.native;
    }

    ret = call(vm, native->arguments, native->nargs, function->magic8);
    if (njs_slow_path(ret == NJS_ERROR)) {
        return ret;
    }

    if (ret == NJS_DECLINED) {
        return NJS_OK;
    }

    previous = njs_function_previous_frame(native);

    njs_vm_scopes_restore(vm, native, previous);

    if (!native->skip) {
        *native->retval = vm->retval;
    }

    njs_function_frame_free(vm, native);

    return NJS_OK;
}