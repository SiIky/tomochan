/* compile with:
 *     cc -lcurl -Os -o tomochan tomochan.c
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <curl/curl.h>

static int usage (char * cmd)
{
    return fprintf(stderr,
            "Usage:\n"
            "\t%s [png|jpg] ID\n"
            "\t%s [png|jpg] FIRST LAST\n",
            cmd, cmd);
}

static bool download_page (CURL * curl, const char * url, const char * filename)
{
    FILE * file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error opening `%s`\n", filename);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL,       url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    curl_easy_perform(curl);

    fclose(file);

    return true;
}

static void read_interval (int interval[2], int argc, char ** argv)
{
    interval[0] = atoi(argv[0]);
    interval[1] = (argc == 2) ?
        atoi(argv[1]) :
        interval[0] ;

    if (interval[0] > interval[1]) {
        int tmp = interval[0];
        interval[0] = interval[1];
        interval[1] = tmp;
    }
}

int main (int argc, char ** argv)
{
    if (argc < 2 || argc > 4)
        goto usage;

    bool jpg = false;
    bool png = false;

    bool imgfmt = (png = (strcasecmp("png", argv[1]) == 0))
               || (jpg = (strcasecmp("jpg", argv[1]) == 0));

    if (argc == 4 && !imgfmt)
        goto usage;

    curl_global_init(CURL_GLOBAL_ALL);
    CURL * curl = curl_easy_init();
    if (curl == NULL)
        goto curl_error;

    curl_easy_setopt(curl, CURLOPT_VERBOSE,       0L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS,    0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

    /* skip command path */
    argc--; argv++;
    if (imgfmt && argc > 1) { /* "(i:(png|jpg))?" */
        argc--;
        argv++;
    }

    int i[2] = {0};
    read_interval(i, argc, argv);

    bool res = true;
    for (; i[0] <= i[1]; i[0]++) {
        char fname[9];
        char url[62];

        sprintf(fname,
                "%04d.%s",
                *i,
                (jpg) ? /* default to PNG */
                "jpg" :
                "png"
               );

        sprintf(url,
                (jpg) ?
                "https://sai-zen-sen.jp/comics/twi4/tomochan/works/%04d.jpg" :
                "https://sai-zen-sen.jp/comics/twi4/tomochan/works/tw/%04d.png",
                *i);

        res = res && download_page(curl, url, fname);
    }

    curl_easy_cleanup(curl);

    return (res) ?
        EXIT_SUCCESS : /* all downloads succeeded */
        EXIT_FAILURE ; /* at least one download failed */

usage:
    usage(*argv);
    goto ko;
curl_error:
    fputs("Error creating a CURL handle", stderr);
    goto ko;
ko:
    return EXIT_FAILURE;
}
