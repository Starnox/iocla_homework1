build:
	gcc -Wall -Wextra tema1.c -g -o tema

clean:
	rm tema

checker:
	python3 checker.py
