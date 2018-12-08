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
    char *units = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    char *end = units + st.st_size;
    int dirty;
    int reduced = 0;

    do {
        dirty = 0;
        char *a = units;
        char *b = units+1;
        while (b < end) {
            if (*b == 0)
                ++b;
            else if (a == b)
                ++b;
            else if (*a == 0)
                ++a;
            else if ((~0x20 & *a) == (~0x20 & *b) && *a != *b) {
                *a = *b = 0;
                dirty = 1;
                ++reduced;
                a = b+1;
                b += 2;
            } else {
                ++a;
                ++b;
            }
        }
    } while (dirty);

    int count = st.st_size - 2*reduced;
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
    write(1, num+i, 16-i);
}
