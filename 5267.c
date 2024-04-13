njs_function_frame_free(njs_vm_t *vm, njs_native_frame_t *native)
{
    njs_native_frame_t  *previous;

    do {
        previous = native->previous;

        /* GC: free frame->local, etc. */

        if (native->size != 0) {
            vm->stack_size -= native->size;
            njs_mp_free(vm->mem_pool, native);
        }

        native = previous;
    } while (native->skip);
}