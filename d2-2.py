from sys import stdin
from itertools import combinations

for s, t in combinations(stdin, 2):
    diff = -1
    for i, p in enumerate(zip(s, t)):
        if p[0] != p[1]:
            if diff >= 0:
                diff = -1
                break
            else:
                diff = i
    if diff >= 0:
        print(s[:diff]+s[diff+1:])
        break
