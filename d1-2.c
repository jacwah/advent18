#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *cont = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *start = cont;
    char *end = cont + st.st_size;
    int freq = 0;
    int *seenlist = mmap(NULL, 1024*1024, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    int seen = 0;

    for (;;) {
        seenlist[seen++] = freq;
        int mod = 0;
        char sgn = *cont++;
        while (*cont != '\n') {
            mod *= 10;
            mod += *cont - '0';
            cont++;
        }
        cont++;
        if (sgn == '-')
            freq -= mod;
        else
            freq += mod;

        for (int i = 0; i < seen; ++i)
            if (seenlist[i] == freq)
                goto done;
        if (cont == end)
            cont = start;
        continue;
done:
        break;
    }

    char num[16];
    int i = 16;
    num[--i] = '\n';
    if (freq == 0) {
        num[--i] = '0';
    } else {
        if (freq < 0) {
            num[--i] = '-';
            freq *= -1;
        }
        while (freq != 0) {
            num[--i] = freq % 10 + '0';
            freq /= 10;
        }
    }
    write(1, num+i, 16-i);
}
