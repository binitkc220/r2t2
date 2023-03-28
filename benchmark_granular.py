#!/usr/bin/env python3
import os
import tqdm
import random

BVH = 1
LIN = 0

ns = range(1, 5, int(1))

for n in tqdm.tqdm(ns):
    print(f"{n}, ", end='', flush=True)
    for method in [BVH, LIN]:
        os.system(f"./bin/release/rt 2 2 {n} {method} 2> /dev/null")
        print(", ", end='', flush=True)
    print()

#Ray_Tracing > bin/release/rt 8 2 100 1 2> /dev/null
