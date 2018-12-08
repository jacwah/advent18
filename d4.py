from sys import stdin
from datetime import datetime
from collections import defaultdict

class Entry:
    BEGIN = 1
    SLEEP = 2
    WAKE = 3

    def __init__(self, s):
        ls = s.split('] ')
        self.time = datetime.strptime(ls[0], '[%Y-%m-%d %H:%M')
        if ls[1].startswith('falls'):
            self.action = self.SLEEP
        elif ls[1].startswith('wakes'):
            self.action = self.WAKE
        else:
            self.action = self.BEGIN
            self.guard = int(ls[1].split()[1][1:])

    def __lt__(self, other):
        return self.time < other.time

    def __str__(self):
        s = str(self.time) + ' ' + str(self.action)
        if self.action == self.BEGIN:
            s += ' ' + str(self.guard)
        return s

entries = sorted(map(Entry, stdin))
sleeptime = defaultdict(int)
sleepmin = defaultdict(lambda: defaultdict(int))
guard = None
for e in entries:
    if e.action == e.BEGIN:
        guard = e.guard
    elif e.action == e.SLEEP:
        start = e.time
    else:
        sleeptime[guard] += (e.time - start).seconds / 60
        minute = start.minute
        while minute != e.time.minute:
            sleepmin[guard][minute] += 1
            minute += 1

mostsleep = max(sleeptime.items(), key=lambda p: p[1])
print(mostsleep)
mins = sleepmin[mostsleep[0]]
bestmin = max(mins.items(), key=lambda p: p[1])
print(bestmin)
print(mostsleep[0]*bestmin[0])

guardmins = map(lambda p: (p[0], max(p[1].items(), key=lambda q: q[1])), sleepmin.items())
guardmin = max(guardmins, key=lambda t: t[1][1])
print(guardmin)
print(guardmin[0]*guardmin[1][0])
