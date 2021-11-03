ht.exe: hash_table.c
	gcc -g -Wall --std=c99 hash_table.c -o ht.exe
	
clean:
	rm *.exe
link:
	gcc -g --std=c99 linked_list.c -o link.exe
