from sys import stdin

twocount = 0
threecount = 0

for id in stdin:
    curr = None
    two = False
    three = False
    count = 0
    for c in sorted(id):
        if c == curr:
            count += 1
        else:
            if count == 2:
                two = True
            elif count == 3:
                three = True
            curr = c
            count = 1
    if count == 2:
        two = True
    elif count == 3:
        three = True
    if two:
        twocount += 1
    if three:
        threecount += 1

check = twocount * threecount
print(f"{check} ({twocount}, {threecount})")
