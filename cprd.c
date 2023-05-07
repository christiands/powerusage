#include <stdio.h>
#include <stdlib.h>

#define HS_COMMON_H
#include "common.h"

const char* CPRD_ENVVAR = "CPRD_DOWNLOAD_PATH";

const size_t LEN_ENVVAR = PATH_MAX;

int main()
{
    int retval = 0;
    
    char* envval = (char*) malloc(sizeof(char) * LEN_ENVVAR); // TODO: malloc error handling
    char* mrfile = (char*) malloc(sizeof(char) * PATH_MAX);
    char* mrpath = (char*) malloc(sizeof(char) * PATH_MAX);
    char* cdpath = (char*) malloc(sizeof(char) * PATH_MAX);
    
    if(!get_envvar(CPRD_ENVVAR, envval, LEN_ENVVAR)) {
        fprintf(stderr, "Couldn't get the environment variable '%s'!\n", CPRD_ENVVAR);
        goto fail;
    }

    if(!get_recentf(envval, mrfile, PATH_MAX)) {
        fprintf(stderr, "Couldn't get the most recent file in '%s'!\n", envval);
        goto fail;
    }

    if(!get_recentp(envval, mrpath, PATH_MAX)) {
        fprintf(stderr, "Couldn't get the most recent path in '%s'!\n", envval);
        goto fail;
    }

    if(!getcwd(cdpath, PATH_MAX)) {
        fprintf(stderr, "Couldn't get the current working directory!\n");
        goto fail;
    }

    conv_dir(cdpath, PATH_MAX);
    strncpy(cdpath + strlen(cdpath), mrfile, PATH_MAX - strlen(cdpath));

    printf("Copying '%s' to '%s'\n", mrpath, cdpath);
    
    if(copy_file(mrpath, cdpath)) {
        fprintf(stderr, "Couldn't copy '%s' to '%s'!\n", mrpath, cdpath);
        goto fail;
    }

    goto cleanup;

 fail:
    retval = -1;
    fprintf(stderr, "Error code: %s\n", strerror(errno));

 cleanup:
    free(envval);
    free(mrfile);
    free(mrpath);

    return retval;
}
