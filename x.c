#include <inttypes.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libx.h"

static size_t fsize(FILE *stream)
{
    long begin, end;

    if ((begin = ftell(stream)) == (long) -1) {
        fprintf(stderr, "Stream is not seekable\n");
        abort();
    }

    if (fseek(stream, 0, SEEK_END))
        abort();

    if ((end = ftell(stream)) == (long) -1)
        abort();

    if (fseek(stream, begin, SEEK_SET))
        abort();

    return (size_t) end - (size_t) begin;
}

struct layer {
    void *data;  /* input data */
    size_t size; /* input size */
} layer[2];

static inline void load_layer(size_t j, FILE *stream)
{
    layer[j].size = fsize(stream);
    if (!(layer[j].data = malloc(layer[j].size))) {
        fprintf(stderr, "Out of memory\n");
        abort();
    }

    if (fread(layer[j].data, 1, layer[j].size, stream) < layer[j].size)
        abort();
    fprintf(stderr, "  Input size: %" PRIu64 " bytes\n", layer[j].size);
}

static inline void save_layer(size_t j, FILE *stream)
{
    fprintf(stderr, "  Output size: %" PRIu64 " bytes\n", layer[j].size);
    if (fwrite(layer[j].data, 1, layer[j].size, stream) < layer[j].size)
        abort();
}

int main(int argc, char *argv[])
{
    FILE *istream = stdin, *ostream = stdout;

    if (strcmp(basename(argv[0]), "unx")) { /* Compress */
        load_layer(0, istream);
        if ((layer[1].data = malloc(8 * layer[0].size)) == NULL)
            abort();

        fprintf(stderr, "Compressing...\n");
        x_init();

        void *end = x_compress(layer[0].data, layer[0].size, layer[1].data);
        layer[1].size = (char *) end - (char *) layer[1].data;

        save_layer(1, ostream);
    } else { /* Decompress */
        load_layer(1, istream);
        if ((layer[0].data = malloc(8 * layer[1].size + 4096)) == NULL)
            abort();

        fprintf(stderr, "Decompressing...\n");
        x_init();

        void *end = x_decompress(layer[1].data, layer[1].size, layer[0].data);
        layer[0].size = (char *) end - (char *) layer[0].data;

        save_layer(0, ostream);
    }

    free(layer[0].data);
    free(layer[1].data);

    fclose(istream);
    fclose(ostream);

    return 0;
}
