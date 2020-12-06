import random

file = open("test0.txt", "w")

k = int(input("K: "))
dimn = int(input("DIMn: "))
dimm = int(input("DIMm: "))

file.write(str(k) + '\n')
file.write(str(dimn) + '\n')
file.write(str(dimm) + '\n')

for i in range(dimn*dimm):
	file.write('\n') if i%dimm == 0 and i!=0 else file.write('')
	file.write(str("{0:.2f}".format(random.uniform(0, 9))) + ' ')
	
file.write('\n\n-1 1 -1\n1 0 1\n-1 1 -1')
