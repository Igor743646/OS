import random
import copy
from random import choice
from string import ascii_uppercase

if __name__ == "__main__":
	test_file = open("test0.txt", 'w')
	ans_file = open("test0.t", 'w')
	test_file.write("a.txt\n")

	count_of_strings = 50

	for _ in range(random.randint(1, count_of_strings)):

		first = random.randint(500, 5000)
		seconds = [str(random.randint(1, 20)) for _ in range(random.randint(1, 5))]

		ans = first
		for i in seconds:
			ans/=int(i)

		test_file.write( str(first) + " " )
		test_file.write( ' '.join(seconds) + "\n")
		ans_file.write( str(ans) + "\n" )

	test_file.write( "0 0")

	test_file.close()