#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define HASH_PRIME 31
#define RESIZE_THRESHOLD 0.2

// typedefs
typedef struct ht HashTable;
typedef struct key_value Pair;

// function declarations
int hash(char * key, int mod);
void * ht_get_pair(HashTable * ht, char * key);
void * ht_get_value(HashTable * ht, char * key);
char ** ht_get_keys(HashTable * ht);
void ht_delete(HashTable * ht, char * key);
void ht_insert(struct ht * ht , char * key, void * value);
void resizeTable(struct ht * ht);
Pair * createPair(char * key, void * value);
struct ht * create_ht(int length);
void die(const char * msg);

struct ht{
	Pair * arr; // Array to store the values
	int length; // The size allocated for arr
	int nmem; // current number of members in the array
	char ** keys; // Array of all the keys entered
} ;

struct key_value{
	char * key;
	void * value;
} ;

void die(const char * msg){
	printf("%s\n", msg);
	exit(-1);
}

int hash(char * key, int mod){
	int h = 0;
	for(int i=0; key[i]!='\0'; i++){
		h = (HASH_PRIME * h + key[i])%mod ;
	}

	h = (HASH_PRIME * h)%mod;

	return h;
}

struct ht * create_ht(int length){
	HashTable * ht = malloc(sizeof(HashTable));
	ht->arr = calloc(length, sizeof(Pair));
	ht->keys = calloc(length, sizeof(char *));
	ht->length = length;
	ht->nmem = 0;

	return ht;
}

Pair * createPair(char * key, void * value){
	Pair * p = malloc(sizeof(Pair));
	p->key = key;
	p->value = value;

	return p;
}

void resizeTable(struct ht * ht){

	// Create a superficial copy of the table
	// this copy is used to get the values and copy them into the new
	// array, while maintaining the old length and thus
	// the old hash function
	HashTable * ht_copy = malloc(sizeof(HashTable));
	memcpy(ht_copy, ht, sizeof(HashTable));

	// By how much the table is going to grow
	int gfactor = 10;
	
	// Create a new table with larger size
	int newLength = gfactor *  ht->length;

	// Set the new size of the hash table
	ht->length = newLength;

	// Allocate a larger chunk of memory to the table
	Pair * newArray = calloc(newLength, sizeof(Pair));
	if(newArray==NULL)
		die("Array resize failed");

	// Assign the new empty array to the table
	ht->arr = newArray;
	ht->nmem = 0;

	char * key;
	void * value;

	// Copy the table members into the new array
	for(int i=0; i < ht->nmem; i++){
		key = ht->keys[i];
		value = ht_get_value(ht_copy, key);
		ht_insert(ht, key, value);
	}

	free(ht_copy);

}

void ht_insert(struct ht * ht , char * key, void * value){

	static bool isResize = false;

	// Check if the array is too full 
	// if it is resize it
	float filledRatio = (ht->nmem + 1)/((float)ht->length);

	if(filledRatio > RESIZE_THRESHOLD){
		isResize = true;
		resizeTable(ht);
		isResize = false;
	}

	// Create a key value pair
	Pair * p = createPair(key, value);

	// Calculate the hash value for the key
	int hashedIndex = hash(key, ht->length);

	// Place the value in the correct index and if spot is taken
	// use linear probing strategy
	int finalIndex = hashedIndex;

	while(1){

		// Is the table entry empty?
		bool isNull = ht->arr[finalIndex].value==NULL;

		// if free space in found stop probing
		if(isNull){
			ht->arr[finalIndex] = *p;
			ht->nmem++;
			break;
		}

		// Does the entry has the same key as the one being entered
		bool same_key = strncmp(ht->arr[finalIndex].key, key, strlen(key))==0;

		// Have we reached the end of the table
		bool at_the_end = finalIndex==(ht->length-1);

		// If we reached an entry with the same key
		// update the entry
		if(same_key){
			ht->arr[finalIndex] = *p;
			break;
		}

		// If we reached the end of the array circle back to the start
		if(at_the_end){
			finalIndex = 0;
			continue;
		}

		finalIndex++;
	}

}

void * ht_get_pair(HashTable * ht, char * key){
	// Calculate the hash value for the key
	int hashedIndex = hash(key, ht->length);
	int finalIndex = hashedIndex;

	// use linear probing to find the actual index
	// stop when correct index is found or out of bounds

	int steps = 0;

	while(1){

		// if we reach an empty slot it mean no such key
		if(ht->arr[finalIndex].value==NULL)
			return NULL;
		
		// Does the entry has the same key as the one being entered
		bool same_key = strncmp(ht->arr[finalIndex].key, key, strlen(key))==0;

		// Have we reached the end of the table
		bool at_the_end = finalIndex==(ht->length-1);

		// If we reached an entry with the same key
		if(same_key){
			break;
		}

		if(at_the_end){
			finalIndex = 0;
			continue;
		}

		if(steps==ht->length-1)
			return NULL;

		finalIndex++;
		steps++;
	}

	// If the index is out of bounds key is not in table
	// and we return NULL
	if(finalIndex == ht->length)
		return NULL;

	Pair * p = &ht->arr[finalIndex];
	return p;
}

char ** ht_get_keys(HashTable * ht){

	// the number of keys should be the same as the 
	// number of entries in the table
	char ** keys = calloc(ht->nmem, sizeof(char *));	

	// loop over the table, fetch the key and add
	// it to the keys array if it's not null
	char * key;
	int key_index = 0;
	for(int i=0; i<ht->length; i++){
		
		key = ht->arr[i].key;
		if(key==NULL) continue;
		keys[key_index] = key;
		key_index++;
		
	}

	return keys;

}

void * ht_get_value(HashTable * ht, char * key){
	// Calculate the hash value for the key
	int hashedIndex = hash(key, ht->length);
	int finalIndex = hashedIndex;

	// use linear probing to find the actual index
	// stop when correct index is found or out of bounds

	int steps = 0;

	while(1){

		// if we reach an empty slot it mean no such key
		if(ht->arr[finalIndex].value==NULL)
			return NULL;
		
		// Does the entry has the same key as the one being entered
		bool same_key = strncmp(ht->arr[finalIndex].key, key, strlen(key))==0;

		// Have we reached the end of the table
		bool at_the_end = finalIndex==(ht->length-1);

		// If we reached an entry with the same key
		if(same_key){
			break;
		}

		if(at_the_end){
			finalIndex = 0;
			continue;
		}

		if(steps==ht->length-1)
			return NULL;

		finalIndex++;
		steps++;
	}

	// If the index is out of bounds key is not in table
	// and we return NULL
	if(finalIndex == ht->length)
		return NULL;

	void * value = ht->arr[finalIndex].value;
	return value;
}

void ht_delete(HashTable * ht, char * key){
	Pair * p = ht_get_pair(ht, key);
	p->key = NULL;
	p->value = NULL;
	ht->nmem--;
}

int main(void){

	int size = 10000000;
	HashTable * ht = create_ht(size);
	int nmem = 1000000;

	char * key;
	char * value;
	for(int i=0; i<nmem; i++){
		key = malloc(32);
		value = malloc(32);
		sprintf(key, "key %d", i);
		sprintf(value, "value %d", i);
		ht_insert(ht, key, value);
	}

	char ** keys = ht_get_keys(ht);

	//clock_t start = clock();
	for(int i=0; i<500000; i++){
		char * key = keys[i];
		//key = malloc(32);
		//sprintf(key, "key %d", i);
		char * value = (char *) ht_get_value(ht, key);
		//printf("%s : %s\n", key, value);
	}

	clock_t length = clock()/CLOCKS_PER_SEC;
	//clock_t length = (end - start)/CLOCKS_PER_SEC ;
	printf("%f\n", (double)length);
	printf("%f\n", (double)length);

}
