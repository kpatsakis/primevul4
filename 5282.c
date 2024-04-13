njs_function_lambda_frame(njs_vm_t *vm, njs_function_t *function,
    const njs_value_t *this, const njs_value_t *args, njs_uint_t nargs,
    njs_bool_t ctor)
{
    size_t                 n, frame_size;
    uint32_t               args_count, value_count, value_size, temp_size;
    njs_value_t            *value, *bound, **new, **temp;
    njs_frame_t            *frame;
    njs_function_t         *target;
    njs_native_frame_t     *native_frame;
    njs_function_lambda_t  *lambda;

    bound = function->bound;

    if (njs_fast_path(bound == NULL)) {
        lambda = function->u.lambda;
        target = function;

    } else {
        target = function->u.bound_target;

        if (njs_slow_path(target->bound != NULL)) {

            /*
             * FIXME: bound functions should call target function with
             * bound "this" and bound args.
             */

            njs_internal_error(vm, "chain of bound function are not supported");
            return NJS_ERROR;
        }

        lambda = target->u.lambda;
    }

    args_count = function->args_offset + njs_max(nargs, lambda->nargs);
    value_count = args_count + njs_max(args_count, lambda->nlocal);

    value_size = value_count * sizeof(njs_value_t *);
    temp_size = lambda->temp * sizeof(njs_value_t *);

    frame_size = value_size + temp_size
                        + ((value_count + lambda->temp) * sizeof(njs_value_t));

    native_frame = njs_function_frame_alloc(vm, NJS_FRAME_SIZE + frame_size);
    if (njs_slow_path(native_frame == NULL)) {
        return NJS_ERROR;
    }

    /* Local */

    new = (njs_value_t **) ((u_char *) native_frame + NJS_FRAME_SIZE);
    value = (njs_value_t *) ((u_char *) new + value_size + temp_size);

    n = value_count + lambda->temp;

    while (n != 0) {
        n--;
        new[n] = &value[n];
        njs_set_invalid(new[n]);
    }

    /* Temp */

    temp = (njs_value_t **) ((u_char *) native_frame + NJS_FRAME_SIZE
                                                     + value_size);

    native_frame->arguments = value;
    native_frame->arguments_offset = value + (function->args_offset - 1);
    native_frame->local = new + args_count;
    native_frame->temp = temp;
    native_frame->function = target;
    native_frame->nargs = nargs;
    native_frame->ctor = ctor;
    native_frame->native = 0;
    native_frame->pc = NULL;

    /* Set this and bound arguments. */
    *native_frame->local[0] = *this;

    if (njs_slow_path(function->global_this
                      && njs_is_null_or_undefined(this)))
    {
        njs_set_object(native_frame->local[0], &vm->global_object);
    }

    if (bound != NULL) {
        n = function->args_offset;
        native_frame->nargs += n - 1;

        if (!ctor) {
            *native_frame->local[0] = *bound;
        }

        bound++;
        n--;

        while (n != 0) {
            *value++ = *bound++;
            n--;
        };
    }

    /* Copy arguments. */

    if (args != NULL) {
        while (nargs != 0) {
            *value++ = *args++;
            nargs--;
        }
    }

    frame = (njs_frame_t *) native_frame;
    frame->exception.catch = NULL;
    frame->exception.next = NULL;
    frame->previous_active_frame = vm->active_frame;

    return NJS_OK;
}