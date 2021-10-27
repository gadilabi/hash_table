ht.exe: hash_table.c
	gcc -g --std=c99 hash_table.c -o ht.exe
	
link:
	gcc -g --std=c99 linked_list.c -o link.exe
