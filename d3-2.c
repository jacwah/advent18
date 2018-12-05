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

struct uf {
    uint16_t *par;
    uint8_t *size;
};

int uf_find(struct uf uf, int x)
{
    while (uf.par[x] != x)
        x = uf.par[x];
    return x;
}

void uf_union(struct uf uf, int x, int y)
{
    x = uf_find(uf, x);
    y = uf_find(uf, y);
    if (x != y) {
        if (uf.size[x] < uf.size[y]) {
            int t = x;
            x = y;
            y = t;
        }
        uf.par[y] = x;
        uf.size[x] += uf.size[y];
    }
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
    int fabsize = 2*size*size;
    int maxcoll = 2048;
    int collsize = 2*maxcoll;
    int ranksize = 2048;
    void *slab = mmap(NULL, fabsize+collsize+ranksize,
            PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    uint16_t *fab = slab;
    struct uf uf;
    uf.par = (uint16_t *)((uint8_t *)fab + fabsize);
    uf.size = (uint8_t *)uf.par + collsize;

    for (int i = 0; i < maxcoll; ++i)
        uf.par[i] = i;

    for (int i = 0; i < ranksize; ++i)
        uf.size[i] = 1;

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

        for (int i = y; i < y+h; ++i) {
            for (int j = x; j < x+w; ++j) {
                int idx = i+size*j;
                int di = fab[idx];
                if (di)
                    uf_union(uf, id, di);
                else
                    fab[idx] = id;
            }
        }
    }

    int iso = 0;
    int r;
    do {
        r = uf_find(uf, ++iso);
    } while (uf.size[r] != 1);

    char buf[16];
    int unused = itos(buf, 15, iso);
    buf[15] = '\n';
    write(1, buf+unused, 16-unused);
}
