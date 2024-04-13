njs_function_native_frame(njs_vm_t *vm, njs_function_t *function,
    const njs_value_t *this, const njs_value_t *args, njs_uint_t nargs,
    njs_bool_t ctor)
{
    size_t              size;
    njs_uint_t          n;
    njs_value_t         *value, *bound;
    njs_native_frame_t  *frame;

    size = NJS_NATIVE_FRAME_SIZE
           + (function->args_offset + nargs) * sizeof(njs_value_t);

    frame = njs_function_frame_alloc(vm, size);
    if (njs_slow_path(frame == NULL)) {
        return NJS_ERROR;
    }

    frame->function = function;
    frame->nargs = function->args_offset + nargs;
    frame->ctor = ctor;
    frame->native = 1;
    frame->pc = NULL;

    value = (njs_value_t *) ((u_char *) frame + NJS_NATIVE_FRAME_SIZE);

    frame->arguments = value;
    frame->arguments_offset = value + function->args_offset;

    bound = function->bound;

    if (bound == NULL) {
        /* GC: njs_retain(this); */
        *value++ = *this;

    } else {
        n = function->args_offset;

        do {
            /* GC: njs_retain(bound); */
            *value++ = *bound++;
            n--;
        } while (n != 0);
    }

    if (args != NULL) {
        memcpy(value, args, nargs * sizeof(njs_value_t));
    }

    return NJS_OK;
}