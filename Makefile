build:
	gcc -Wall -Wextra tema1.c -g -o tema1

clean:
	rm tema

checker:
	python3 checker.py
