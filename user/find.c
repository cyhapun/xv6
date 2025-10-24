#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// ---------- Convert string to lowercase ----------
void str_to_lower(char *dest, const char *src, int max_len)
{
    if (!dest || !src || max_len <= 0)
        return;
    int i = 0;
    while (i < max_len - 1 && src[i])
    {
        char c = src[i];
        if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        dest[i] = c;
        i++;
    }
    dest[i] = 0;
}

// ---------- Append string to another (xv6 safe) ----------
void strcat_xv6(char *dst, const char *src)
{
    int i = 0;
    while (dst[i])
        i++; // tìm null terminator của dst
    int j = 0;
    while (src[j])
    {
        dst[i++] = src[j++];
    }
    dst[i] = 0;
}

// ---------- Recursive find ----------
void find(char *path, char *target)
{
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;

        // skip "." and ".."
        if (de.name[0] == '.' && (de.name[1] == 0 ||
                                  (de.name[1] == '.' && de.name[2] == 0)))
            continue;

        char buf[512];
        strcpy(buf, path);
        int len = strlen(buf);
        if (buf[len - 1] != '/')
            buf[len++] = '/';
        buf[len] = 0;
        strcat_xv6(buf, de.name);

        if (stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        // Convert names to lowercase for case-insensitive compare
        char lower_name[128];
        str_to_lower(lower_name, de.name, sizeof(lower_name));
        char lower_target[128];
        str_to_lower(lower_target, target, sizeof(lower_target));

        if (st.type == T_FILE)
        {
            if (strcmp(lower_name, lower_target) == 0)
                printf("%s\n", buf);
        }
        else if (st.type == T_DIR)
        {
            find(buf, target); // recursive
        }
    }
    close(fd);
}

// ---------- main ----------
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
