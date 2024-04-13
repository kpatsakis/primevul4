njs_function_prototype_call(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t unused)
{
    njs_int_t           ret;
    njs_function_t      *function;
    const njs_value_t   *this;
    njs_native_frame_t  *frame;

    if (!njs_is_function(&args[0])) {
        njs_type_error(vm, "\"this\" argument is not a function");
        return NJS_ERROR;
    }

    if (nargs > 1) {
        this = &args[1];
        nargs -= 2;

    } else {
        this = (njs_value_t *) &njs_value_undefined;
        nargs = 0;
    }

    frame = vm->top_frame;

    /* Skip the "call" method frame. */
    frame->skip = 1;

    function = njs_function(&args[0]);

    ret = njs_function_frame(vm, function, this, &args[2], nargs, 0);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_function_frame_invoke(vm, frame->retval);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    return NJS_DECLINED;
}