local void parallel_compress(void)
{
    long seq;                       /* sequence number */
    struct space *curr;             /* input data to compress */
    struct space *next;             /* input data that follows curr */
    struct space *hold;             /* input data that follows next */
    struct space *dict;             /* dictionary for next compression */
    struct job *job;                /* job for compress, then write */
    int more;                       /* true if more input to read */
    unsigned hash;                  /* hash for rsyncable */
    unsigned char *scan;            /* next byte to compute hash on */
    unsigned char *end;             /* after end of data to compute hash on */
    unsigned char *last;            /* position after last hit */
    size_t left;                    /* last hit in curr to end of curr */
    size_t len;                     /* for various length computations */

    /* if first time or after an option change, setup the job lists */
    setup_jobs();

    /* start write thread */
    writeth = launch(write_thread, NULL);

    /* read from input and start compress threads (write thread will pick up
       the output of the compress threads) */
    seq = 0;
    next = get_space(&in_pool);
    next->len = readn(g.ind, next->buf, next->size);
    hold = NULL;
    dict = NULL;
    scan = next->buf;
    hash = RSYNCHIT;
    left = 0;
    do {
        /* create a new job */
        job = MALLOC(sizeof(struct job));
        if (job == NULL)
            bail("not enough memory", "");
        job->calc = new_lock(0);

        /* update input spaces */
        curr = next;
        next = hold;
        hold = NULL;

        /* get more input if we don't already have some */
        if (next == NULL) {
            next = get_space(&in_pool);
            next->len = readn(g.ind, next->buf, next->size);
        }

        /* if rsyncable, generate block lengths and prepare curr for job to
           likely have less than size bytes (up to the last hash hit) */
        job->lens = NULL;
        if (g.rsync && curr->len) {
            /* compute the hash function starting where we last left off to
               cover either size bytes or to EOF, whichever is less, through
               the data in curr (and in the next loop, through next) -- save
               the block lengths resulting from the hash hits in the job->lens
               list */
            if (left == 0) {
                /* scan is in curr */
                last = curr->buf;
                end = curr->buf + curr->len;
                while (scan < end) {
                    hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
                    if (hash == RSYNCHIT) {
                        len = scan - last;
                        append_len(job, len);
                        last = scan;
                    }
                }

                /* continue scan in next */
                left = scan - last;
                scan = next->buf;
            }

            /* scan in next for enough bytes to fill curr, or what is available
               in next, whichever is less (if next isn't full, then we're at
               the end of the file) -- the bytes in curr since the last hit,
               stored in left, counts towards the size of the first block */
            last = next->buf;
            len = curr->size - curr->len;
            if (len > next->len)
                len = next->len;
            end = next->buf + len;
            while (scan < end) {
                hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
                if (hash == RSYNCHIT) {
                    len = (scan - last) + left;
                    left = 0;
                    append_len(job, len);
                    last = scan;
                }
            }
            append_len(job, 0);

            /* create input in curr for job up to last hit or entire buffer if
               no hits at all -- save remainder in next and possibly hold */
            len = (job->lens->len == 1 ? scan : last) - next->buf;
            if (len) {
                /* got hits in next, or no hits in either -- copy to curr */
                memcpy(curr->buf + curr->len, next->buf, len);
                curr->len += len;
                memmove(next->buf, next->buf + len, next->len - len);
                next->len -= len;
                scan -= len;
                left = 0;
            }
            else if (job->lens->len != 1 && left && next->len) {
                /* had hits in curr, but none in next, and last hit in curr
                   wasn't right at the end, so we have input there to save --
                   use curr up to the last hit, save the rest, moving next to
                   hold */
                hold = next;
                next = get_space(&in_pool);
                memcpy(next->buf, curr->buf + (curr->len - left), left);
                next->len = left;
                curr->len -= left;
            }
            else {
                /* else, last match happened to be right at the end of curr,
                   or we're at the end of the input compressing the rest */
                left = 0;
            }
        }

        /* compress curr->buf to curr->len -- compress thread will drop curr */
        job->in = curr;

        /* set job->more if there is more to compress after curr */
        more = next->len != 0;
        job->more = more;

        /* provide dictionary for this job, prepare dictionary for next job */
        job->out = dict;
        if (more && g.setdict) {
            if (curr->len >= DICT || job->out == NULL) {
                dict = curr;
                use_space(dict);
            }
            else {
                dict = get_space(&dict_pool);
                len = DICT - curr->len;
                memcpy(dict->buf, job->out->buf + (job->out->len - len), len);
                memcpy(dict->buf + len, curr->buf, curr->len);
                dict->len = DICT;
            }
        }

        /* preparation of job is complete */
        job->seq = seq;
        Trace(("-- read #%ld%s", seq, more ? "" : " (last)"));
        if (++seq < 1)
            bail("input too long: ", g.inf);

        /* start another compress thread if needed */
        if (cthreads < seq && cthreads < g.procs) {
            (void)launch(compress_thread, NULL);
            cthreads++;
        }

        /* put job at end of compress list, let all the compressors know */
        possess(compress_have);
        job->next = NULL;
        *compress_tail = job;
        compress_tail = &(job->next);
        twist(compress_have, BY, +1);
    } while (more);
    drop_space(next);

    /* wait for the write thread to complete (we leave the compress threads out
       there and waiting in case there is another stream to compress) */
    join(writeth);
    writeth = NULL;
    Trace(("-- write thread joined"));
}
