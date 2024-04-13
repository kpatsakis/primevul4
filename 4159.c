njs_array_destroy(njs_vm_t *vm, njs_array_t *array)
{
    if (array->data != NULL) {
        njs_mp_free(vm->mem_pool, array->data);
    }

    /* TODO: destroy keys. */

    njs_mp_free(vm->mem_pool, array);
}