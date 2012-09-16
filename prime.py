#!/usr/bin/env python
#direct translation from xr
#terribly unpythonic, obviously

limit = 10000
x = 2
numbers = [0, 0]
primes = []
num_primes = 0
while (x < limit):
    numbers.append(x)
    x = x + 1

p = 2
marked_all = 0
while (marked_all == 0):
    #"p = " show; p string showln
    #mark multiples of p
    mp = p
    while (mp < limit):
        if (numbers[mp] > p):
            numbers[mp] = 0
            #"marking: " show; mp string showln
        mp = mp + p

    #find first number greater than p not marked
    i = 0
    inum = 0
    marked = 0
    while (i < limit and marked != 1):
        inum = numbers[i]
        #"is inum > p? (" show; inum string show; " > " show; p string show; ")" showln
        if (inum > p):
           marked = 1
           p = inum
           #"restarting with p = " show; p string showln
        i = i + 1
    if (marked == 0):
        #"marked all" showln
        marked_all = 1
#spit out primes
n = 0
num = 0
while (n < limit):
    num = numbers[n]
    if (num > 0):
        primes.append(num)
        num_primes = num_primes + 1
    n = n + 1

#print primes
pc = 0
while (pc < num_primes):
    print primes[pc]
    pc = pc + 1
