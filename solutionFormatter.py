import sys

if len(sys.argv) != 4:
    print "Invalid number of input parameters"
    print "First argument -> Input file"
    print "Second argument -> outputfile"
    print "Sort key -> Enter \"t\" to sort by threshold, Enter \"s\" to sort by span, Enter \"q\" to sort by q (shape size)"

inputData = open(sys.argv[1])

data = []

for line in inputData:
    out = ""
    parts = line.split()
    shape = long(parts[0])
    threshold = int(parts[1])
    for x in reversed('{0:00b}'.format(shape)):
        if x == '1':
            out += "#"
        else:
            out += "-"
    data.append((out,threshold))

inputData.close()

if sys.argv[3] == "t":
    data.sort(cmp=lambda (val1, t1),(val2, t2): cmp(t2,t1))
elif sys.argv[3] == "s":
    data.sort(cmp=lambda (val1, t1),(val2, t2): cmp(len(val1),len(val2)))
elif sys.argv[3] == "q":
    data.sort(cmp=lambda (val1, t1),(val2, t2): cmp(reduce(lambda x,y: x+1 if y=='#' else x, val1, 0),reduce(lambda x,y: x+1 if y=='#' else x, val2, 0)))

else:
    print "Invalid key given. Must be one of three values: t, s or q"

output = open(sys.argv[2], 'w')

for (val,t) in data:
    output.write(str(val) + ' ' + str(t) + '\n')

output.close()
