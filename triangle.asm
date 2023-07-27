triangle:adj -3
stl 1
stl 2
ldc 1
shl
ldl 3
sub
brlz skip
ldl 3
ldl 2
sub
stl 2
skip: ldl 2
brz one
ldl 3
adc -1
stl 0
adj -1
ldl 1
stl 0
ldl 3
adc -1
call triangle
ldl 1
stl 0
stl 1
ldl 3
call triangle
adj 1
ldl 0
add
ldl 1
adj 3
return
one: ldc 1
ldl 1
adj 3
return
;
count: data 10
result: data 0