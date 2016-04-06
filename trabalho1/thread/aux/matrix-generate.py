import sys
from random import randint

if len(sys.argv) < 3:
    print ("Uso correto:\n\tpython matrix-generate.py <num-linhas> <num-colunas>")
    sys.exit()

print ("LINHAS =", sys.argv[1])
print ("COLUNAS =", sys.argv[2])

for linha in range(int(sys.argv[1])):
    for coluna in range(int(sys.argv[2])):
         print (randint(0,100), end=" ")
    print ('\n', end="")
