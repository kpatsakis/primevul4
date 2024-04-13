int imagetopgx(opj_image_t * image, const char *outfile)
{
    int w, h;
    int i, j, fails = 1;
    unsigned int compno;
    FILE *fdest = NULL;

    for (compno = 0; compno < image->numcomps; compno++) {
        opj_image_comp_t *comp = &image->comps[compno];
        char bname[256]; /* buffer for name */
        char *name = bname; /* pointer */
        int nbytes = 0;
        size_t res;
        const size_t olen = strlen(outfile);
        const size_t dotpos = olen - 4;
        const size_t total = dotpos + 1 + 1 + 4; /* '-' + '[1-3]' + '.pgx' */

        if (outfile[dotpos] != '.') {
            /* `pgx` was recognized but there is no dot at expected position */
            fprintf(stderr, "ERROR -> Impossible happen.");
            goto fin;
        }
        if (total > 256) {
            name = (char*)malloc(total + 1);
            if (name == NULL) {
                fprintf(stderr, "imagetopgx: memory out\n");
                goto fin;
            }
        }
        strncpy(name, outfile, dotpos);
        sprintf(name + dotpos, "_%u.pgx", compno);
        fdest = fopen(name, "wb");
        /* don't need name anymore */

        if (!fdest) {

            fprintf(stderr, "ERROR -> failed to open %s for writing\n", name);
            if (total > 256) {
                free(name);
            }
            goto fin;
        }

        w = (int)image->comps[compno].w;
        h = (int)image->comps[compno].h;

        fprintf(fdest, "PG ML %c %d %d %d\n", comp->sgnd ? '-' : '+', comp->prec,
                w, h);

        if (comp->prec <= 8) {
            nbytes = 1;
        } else if (comp->prec <= 16) {
            nbytes = 2;
        } else {
            nbytes = 4;
        }

        for (i = 0; i < w * h; i++) {
            /* FIXME: clamp func is being called within a loop */
            const int val = clamp(image->comps[compno].data[i],
                                  (int)comp->prec, (int)comp->sgnd);

            for (j = nbytes - 1; j >= 0; j--) {
                int v = (int)(val >> (j * 8));
                unsigned char byte = (unsigned char)v;
                res = fwrite(&byte, 1, 1, fdest);

                if (res < 1) {
                    fprintf(stderr, "failed to write 1 byte for %s\n", name);
                    if (total > 256) {
                        free(name);
                    }
                    goto fin;
                }
            }
        }
        if (total > 256) {
            free(name);
        }
        fclose(fdest);
        fdest = NULL;
    }
    fails = 0;
fin:
    if (fdest) {
        fclose(fdest);
    }

    return fails;
}
