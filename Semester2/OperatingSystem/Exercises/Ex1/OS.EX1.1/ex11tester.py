__author__ = "David Samuelson"

import sys
import string
import random
from random import randint
import re
import os.path
import subprocess

textLen = 512

def main():
    numOfTests = 10000
    res = True
    for i in range(numOfTests):
        if not test():
            res = False
            print 'Failed after {} tests'.format(i)
            break
        if i % 100 == 0:
            print 'Finished {} tests'.format(i)
    if res:
        print 'Successfully passed all {} tests!'.format(numOfTests)

def test():
    ftype = random.choice('123')
    if not os.path.isfile('comp.out'):
        print 'comp.out does not exist'
        return False

    # generate two strings by the chosen type
    (s1,s2) = generate(ftype)

    # generate files containing the strings
    fname1 = 'firstfile'
    fname2 = 'secondfile'
    fout = 'outputfile'

    file1 = open(fname1, 'w')
    file2 = open(fname2, 'w')
    file1.write(s1)
    file2.write(s2)
    file1.close()
    file2.close()

    # test the program.

    out = open(fout, 'w')
    retval = subprocess.call(['./comp.out', fname1, fname2], stdout = out, stderr = out)
    out.close()
    retval = str(retval)

    if retval != ftype:
        print 'Test failed: expected output - {0}, actual output: {1}'.format(ftype, retval)
        print 'Details: check firstfile and secondfile, which were the checked files, and outputfile, which holds the output.'
        return False

    # remove the files.
    os.remove(fname1)
    os.remove(fname2)
    os.remove(fout)
    return True

def generate(arg):
    funcDict = {'1' : genEqual, '2': genSimilar, '3' : genDiff}
    if not funcDict.has_key(arg):
        print 'Unknown argument {}'.format(arg)
        return None

    return funcDict[arg]()

def genEqual():
    len = randint(0,textLen)
    #print 'Length of the files: {}'.format(len)
    letters = string.printable
    str = ''.join(random.choice(letters) for i in range(len))
    return (str,str)

def genDiff():
    len1 = randint(0,textLen)
    len2 = randint(0,textLen)
    letters = string.printable
    s1 = ''.join(random.choice(letters) for i in range(len1))
    s2 = ''.join(random.choice(letters) for i in range(len2))

    if isSimilar(s1,s2):
        return genDiff()

    return (s1,s2)

def genSimilar():
    (s1,s2) = genEqual()

    s1 = randomllyAddWhitespaces(s1)
    s2 = randomllyAddWhitespaces(s2)

    # now turn some characters to uppercase / lowercase
    s1 = randomllyChangeCases(s1)
    s2 = randomllyChangeCases(s2)

    # generate again if failed or created equal strings
    if not isSimilar(s1,s2) or s1 == s2:
        return genSimilar()

    return (s1,s2)

def insertChar(str, pos, ch ):
    str   =  str[:pos] + ch + str[pos:] 
    return str

def randomllyAddWhitespaces(str):
    whitespaces = randint(0, len(str)/2)
    indexes = random.sample(range(len(str)), whitespaces)
    for i in indexes:
        ch = random.choice(' \n')
        str = insertChar(str, i, ch)
    return str

def randomllyChangeCases(str):
    charsToChange = randint(0,len(str)/2)
    indexes = random.sample(range(len(str)), charsToChange)

    chars = list(str)
    
    for i in indexes:
        chars[i] = random.choice([chars[i].upper(), chars[i].lower()])

    return ''.join(chars)

def isSimilar(s1,s2):
    if s1 == s2:
        return True
    n1 = re.sub('[\n ]', '', s1)
    n2 = re.sub('[\n ]', '', s2)
    n1 = n1.lower()
    n2 = n2.lower()
    return n1 == n2

if __name__ == "__main__":
    main()

