IN
MOV_STOMEM 0
PUSH 0
MOV_STOMEM 1
PUSH 1
MOV_STOMEM 2
PUSH 0
MOV_STOMEM 3
LABEL 0
MOV_MEMTOS 3
MOV_MEMTOS 0
JL 1
JUMP 2
LABEL 1
MOV_MEMTOS 1
MOV_MEMTOS 2
ADD
MOV_STOMEM 4
MOV_MEMTOS 4
OUT
MOV_MEMTOS 2
MOV_STOMEM 1
MOV_MEMTOS 4
MOV_STOMEM 2
MOV_MEMTOS 3
PUSH 1
ADD
MOV_STOMEM 3
JUMP 0
LABEL 2
PUSH 0
MOV_STOMEM 3
LABEL 3
PUSH 2
MOV_MEMTOS 3
MUL
PUSH 6
JL 4
JUMP 5
LABEL 4
PUSH 111
OUT
MOV_MEMTOS 3
PUSH 1
ADD
MOV_STOMEM 3
JUMP 3
LABEL 5
