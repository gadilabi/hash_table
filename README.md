# Hash Table
This is a simple implemetation of a generic hash table in c.
The keys must be strings but the values can be anything you wish.
In the case of a collision it uses linear probing to find an empty spot.
When the table is 20% full is is resized by 10, these two values can be
easily changed by changing the appropriate macros.

The hash table DOES NOT COPY the values inserted which means that:
* Every key and value inserted into the table must already be on the heap
otherwise they will be lost once the function call returns.
* You must not free any value you have inserted into the table, and when the table is no longer needed use the designated function to free all entries in the table. 

## Files
hash\_table.c 
hash\_table.h

## Structs
```c
typdef struct key_value{
	char * key;
	void * value;
} Pair;

typedef struct ht{
	Pair * arr; // Array to store the values
	int length; // The size allocated for arr
	int nmem; // current number of members in the array
	void (* usr_free)(void * garbage_value); // custom free function
} HashTable;
```

## Functions
```c
int hash(char * key, int mod);
void * ht_get_pair(HashTable * ht, char * key);
void * ht_get_value(HashTable * ht, char * key);
char ** ht_get_keys(HashTable * ht);
int ht_delete(HashTable * ht, char * key);
void ht_insert(struct ht * ht , char * key, void * value);
void resizeTable(struct ht * ht);
struct ht * ht_create(int length);
void ht_set_free(struct ht * ht, void (* usr_free)(void * garbage_value));
void ht_free(HashTable * ht);
static void die(const char * msg);
```

## Description
First a hash table must be created using the ht\_create function.
This function takes one parameter which is the initial size of the table.
Notice that this is not the size in bytes but the number of key-value 
pairs the table can hold.
The size of the table is dynamic so this number will be updated
as the table is being filled, however the process of resizing the table
which includes the need to copy over every entry is very expensive, and 
so it is important to choose an appropriate initial size in order to 
minimize the number of resizes. It should also be noted that a table of
size N will actually be resized when N/5 entries will be inserted. This is because
when using linear probing, if we wait until the table is full the operations will be O(n) instead of O(1).

Once the table is created you can insert key-value pairs into it using the
ht\_insert function.
Notice that both the key and the value should be allocated onto the heap 
when entered into the table as no copies are being made on insertion.

You can retrieve entries using the different ht\_get\_\* functions.
The ht\_get\_pair function retrieves a key-value pair given a key.
The ht\_get\_value function retrieves a value given a key.
The ht\_get\_keys function retrieves an array of all keys in the table.  

Entries can be deleted from the table using the ht\_delete function,
which deletes an entry given a key.
Notice that the values deleted are also freed, and so cannot be used 
anymore.

The set\_free function sets a the usr\_free field in the hash table struct.
It takes the hash table to be updated and function which will be set as
the usr\_free field.
When this field is set it will be used to free the value of every entry
which is either deleted or free when using the ht\_free function.

The ht\_free function frees all the memory used for the table.
When the usr\_free field is set, it will use this function to free the
value. This is needed when the value itself is complex and contains
pointers that need freeing.

Finally there are functions which are not a part of the api but are extremely important and that the user can replace or modify their behaviour.

The hash function is obviously of paramount importance to an efficient 
hash table.
It is extremely easy to replace the hash function without the need to changeanything else in the code.

The second function 






