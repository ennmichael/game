#!/usr/bin/env python3.6

from typing import List, Iterable
import glob
import io


DIRS = ['src', 'include', 'engine/src', 'engine/include/engine']


def source_files(directory: str) -> List[str]:
    return (glob.glob(f'{directory}/*.cpp')
            + glob.glob(f'{directory}/*.h'))


def fuck_bjarne(line: str) -> str:
    def fix_word(word: str) -> str:
        if word and word[0].isupper():
            for i in all_indices(word, '_'):
                word = remove_index(word, i)
                word = upper_index(word, i)

        return word

    words = line.split()
    return ' '.join(fix_word(w) for w in words);


def upper_index(s: str, i: int) -> str:
    return s[:i] + s[i].upper() + s[i+1:]


def remove_index(s: str, i: int) -> str:
    return s[:i] + s[i+1:]


def all_indices(s: str, sub: str) -> Iterable[int]:
    while True:
        try:
            i = s.index(sub)
            s = s[i+1:]
            yield i
        except ValueError:
            break


files = sum((source_files(d) for d in DIRS), [])


