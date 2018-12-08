#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define NORM(c) (~0x20 & (c))

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *units = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *copy = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    char *uend = units + st.st_size;
    char *cend = copy + st.st_size;
    char types[128] = {};
    int minlen = -1;
    char mintype = 0;

    for (char *unit = units; unit != uend; ++unit)
        types[NORM(*unit)] = 1;

    for (int type = 0; type < 128; ++type) {
        int num = st.st_size;
        if (!types[type])
            continue;

        for (int i = 0; i < st.st_size; ++i)
            if (NORM(units[i]) == type) {
                copy[i] = 0;
                --num;
            } else
                copy[i] = units[i];

        int dirty;
        int reduced = 0;
        do {
            dirty = 0;
            char *a = copy;
            char *b = copy+1;
            while (b < cend) {
                if (*b == 0)
                    ++b;
                else if (a == b)
                    ++b;
                else if (*a == 0)
                    ++a;
                else if (NORM(*a) == NORM(*b) && *a != *b) {
                    *a = *b = 0;
                    num -= 2;
                    dirty = 1;
                    a = b+1;
                    b += 2;
                } else {
                    ++a;
                    ++b;
                }
            }
        } while (dirty);

        if (minlen < 0 || num < minlen) {
            mintype = type;
            minlen = num;
        }
    }

    int count = minlen;
    char num[16];
    int i = 16;
    num[--i] = '\n';
    if (count == 0) {
        num[--i] = '0';
    } else {
        if (count < 0) {
            num[--i] = '-';
            count *= -1;
        }
        while (count != 0) {
            num[--i] = count % 10 + '0';
            count /= 10;
        }
    }
    num[--i] = mintype;
    write(1, num+i, 16-i);
}
