free_berdata(struct berval **array)
{
    int i;

    if (array != NULL) {
        for (i = 0; array[i] != NULL; i++) {
            if (array[i]->bv_val != NULL)
                free(array[i]->bv_val);
            free(array[i]);
        }
        free(array);
    }
}
