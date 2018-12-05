#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>

int stoi(char **s)
{
    int i = 0;

    while (**s <= '9' && **s >= '0') {
        i *= 10;
        i += **s - '0';
        (*s)++;
    }

    return i;
}

int itos(char *s, int l, int i)
{
    if (i == 0) {
        s[--l] = '0';
    } else {
        if (i < 0) {
            s[--l] = '-';
            i *= -1;
        }
        while (i != 0) {
            s[--l] = i % 10 + '0';
            i /= 10;
        }
    }

    return l;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *file = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *end = file + st.st_size;
    int size = 2048;
    uint16_t *fab = mmap(NULL, 2*size*size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);

    while (file < end) {
        file++;
        int id = stoi(&file);
        file += 3;
        int x = stoi(&file);
        file++;
        int y = stoi(&file);
        file += 2;
        int w = stoi(&file);
        file++;
        int h = stoi(&file);
        file++;

        for (int i = y; i < y+h; ++i)
            for (int j = x; j < x+w; ++j)
                fab[i+size*j] += 1;
    }

    int count = 0;
    for (int i = 0; i < size*size; ++i)
        if (fab[i] > 1)
            count++;

    char buf[16];
    int unused = itos(buf, 15, count);
    buf[15] = '\n';
    write(1, buf+unused, 16-unused);
}
