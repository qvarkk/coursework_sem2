default:
	gcc main.c -ljson-c -o main

test:
	gcc test.c -ljson-c -o test