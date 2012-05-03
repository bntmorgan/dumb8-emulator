AFC eax #1
SOU esp esp eax
AFC eax #2
PSH eax
AFC eax #3
PSH eax
AFC eax #2
PSH eax
POP ebx
POP eax
MUL eax eax ebx
PSH eax
POP ebx
POP eax
ADD eax eax ebx
PSH eax
POP eax
COP [ebp]-1 eax
PRI [ebp]-1
