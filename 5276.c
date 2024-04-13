njs_function_rest_parameters_init(njs_vm_t *vm, njs_native_frame_t *frame)
{
    uint32_t     length;
    njs_uint_t   nargs, n, i;
    njs_array_t  *array;
    njs_value_t  *rest_arguments;

    nargs = frame->nargs;
    n = frame->function->u.lambda->nargs;
    length = (nargs >= n) ? (nargs - n + 1) : 0;

    array = njs_array_alloc(vm, 1, length, 0);
    if (njs_slow_path(array == NULL)) {
        return NJS_ERROR;
    }

    for (i = 0; i < length; i++) {
        array->start[i] = frame->arguments[i + n - 1];
    }

    rest_arguments = njs_mp_alloc(vm->mem_pool, sizeof(njs_value_t));
    if (njs_slow_path(rest_arguments == NULL)) {
        return NJS_ERROR;
    }

    /* GC: retain. */
    njs_set_array(rest_arguments, array);

    vm->top_frame->local[n] = rest_arguments;

    return NJS_OK;
}