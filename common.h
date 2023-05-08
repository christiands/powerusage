#ifdef HS_COMMON_H

#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __linux__
#include <sys/sendfile.h>
#include <linux/limits.h>
#endif // __linux__

#ifdef __APPLE__
#include <sys/syslimits.h>
#endif // __APPLE__

char* conv_dir(char* dir, size_t dirsz)
{
    size_t end = strlen(dir);

    if(end <= 1 || dir[end - 1] == '/') {
        return dir;
    }

    if(end + 1 > dirsz) {
        return NULL;
    }

    dir[end] = '/';
    dir[end + 1] = '\0';

    return dir;
}


char* get_recentf(const char* dir, char* buf, size_t bufsz)
{
    DIR* d;
    
    if(!(d = opendir(dir))) {
        return NULL;
    }


    struct dirent* c_entry, b_entry;
    struct stat c_stat;
    struct timespec b_time = { 0, 0L };

    size_t dirlen = strlen(dir), filelen;
    char* dirpath = (char*) malloc(sizeof(char) * PATH_MAX), *diroff;


    strncpy(dirpath, dir, dirlen);
    conv_dir(dirpath, PATH_MAX);
    dirlen = strlen(dirpath);

    diroff = dirpath + dirlen;


    while((c_entry = readdir(d))) {
        strncpy(diroff, c_entry->d_name, PATH_MAX - dirlen);
        dirpath[strlen(dirpath)] = '\0';

        if(stat(dirpath, &c_stat)) {
            goto fail;
        }

        int is_file = (c_stat.st_mode & S_IFMT) == S_IFREG;
#ifdef __APPLE__
        int is_newer = c_stat.st_mtime == b_time.tv_sec ?
            c_stat.st_mtimensec > b_time.tv_nsec :
            c_stat.st_mtime > b_time.tv_sec;
        if(is_newer && is_file) {
            b_entry = *c_entry;
            b_time.tv_sec = c_stat.st_mtime;
            b_time.tv_nsec = c_stat.st_mtimensec;
        }
#else
        int is_newer = c_stat.st_mtim.tv_sec == b_time.tv_sec ?
            c_stat.st_mtim.tv_nsec > b_time.tv_nsec :
            c_stat.st_mtim.tv_sec > b_time.tv_sec;
        if(is_newer && is_file) {
            b_entry = *c_entry;
            b_time = c_stat.st_mtim;
        }
#endif // __APPLE__
    }

    
    filelen = strlen(b_entry.d_name);

    if(bufsz < filelen + 1) {
        goto fail;
    }

    memcpy(buf, b_entry.d_name, filelen + 1);

    goto cleanup;
    

 fail:
    buf = NULL;
    
 cleanup:
    closedir(d);
    free(dirpath);

    return buf;
}

char* get_recentp(const char* dir, char* buf, size_t bufsz)
{
    char* dirpath = (char*) malloc(sizeof(char) * PATH_MAX);
    char* filename = (char*) malloc(sizeof(char) * PATH_MAX);

    strncpy(dirpath, dir, PATH_MAX);
    
    if(!conv_dir(dirpath, PATH_MAX)) {
        goto fail;
    }
    
    if(!get_recentf(dirpath, filename, PATH_MAX)) {
        goto fail;
    }

    strncpy(dirpath + strlen(dirpath), filename, PATH_MAX - strlen(dirpath));
    memcpy(buf, dirpath, strlen(dirpath) + 1);

    goto cleanup;

 fail:
    buf = NULL;

 cleanup:
    free(dirpath);
    free(filename);

    return buf;
}

char* get_envvar(const char* env, char* buf, size_t bufsz)
{
    char* envptr = getenv(env);
    
    if(!envptr) {
        return NULL;
    }
    
    if(bufsz < strlen(envptr) + 1) {
        return NULL;
    }
        

    strncpy(buf, envptr, bufsz);

    return buf;
}

// Returns 0 on good, returns 1 on fail
int copy_file(const char* from, const char* to)
{
    int retval = 0;
    
    FILE* f_from = fopen(from, "r");
    if(!f_from) {
        return 1;
    }

    FILE* f_to = fopen(to, "w");
    if(!f_from) {
        fclose(f_from);
        return 1;
    }

    fseek(f_from, 0L, SEEK_END);
    size_t sz_from = ftell(f_from);
    fseek(f_from, 0L, SEEK_SET);

    void* buffer = malloc(sz_from);
    size_t read = fread(buffer, 1, sz_from, f_from);
    if(read != sz_from) {
        goto fail;
    }

    size_t written = fwrite(buffer, 1, sz_from, f_to);
    if(written != sz_from) {
        goto fail;
    }

    goto cleanup;
    
 fail:
    retval = 1;
    
 cleanup:
    fclose(f_from);
    fclose(f_to);

    free(buffer);

    return retval;
}


int move_file(const char* from, const char* to)
{
    if(copy_file(from, to)) {
        return 1;
    }

    if(unlink(from)) {
        return 1;
    }

    return 0;
}

#endif // HS_COMMON_H
