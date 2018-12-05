#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *cont = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *end = cont + st.st_size;
    int freq = 0;

    while (cont < end) {
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
