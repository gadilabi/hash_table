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
int ht_delete(HashTable * ht, char * key);
void ht_insert(struct ht * ht , char * key, void * value);
void resizeTable(struct ht * ht);
struct ht * ht_create(int length);
void ht_set_free(struct ht * ht, void (* usr_free)(void * garbage_value));
void ht_free(HashTable * ht);
static void die(const char * msg);

struct ht{
	Pair * arr; // Array to store the values
	int length; // The size allocated for arr
	int nmem; // current number of members in the array
	void (* usr_free)(void * garbage_value); // custom free function
};

struct key_value{
	char * key;
	void * value;
} ;

static void die(const char * msg){
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

struct ht * ht_create(int length){
	HashTable * ht = malloc(sizeof(HashTable));
	ht->arr = calloc(length, sizeof(Pair));
	ht->length = length;
	ht->nmem = 0;
	ht->usr_free = NULL;

	return ht;
}


void ht_set_free(struct ht * ht, void (* usr_free)(void * garbage_value)){
	ht->usr_free = usr_free;
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
			ht->arr[finalIndex].key = key;
			ht->arr[finalIndex].value = value;
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
			ht->arr[finalIndex].key = key;
			ht->arr[finalIndex].value = value;
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

	// If the table is empty return NULL
	if(ht->nmem==0)
		return NULL;

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

	// If table is empty return NULL
	if(ht->nmem==0)
		return NULL;

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

	// If the table is empty return NULL
	if(ht->nmem==0)
		return NULL;

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

int ht_delete(HashTable * ht, char * key){

	// If table is empty return -1
	if(ht->nmem == 0)
		return -1;

	Pair * p = ht_get_pair(ht, key);

	// If no such key then return -1
	if(p==NULL) return -1;

	// If the entry is found we need to free the value
	// and the key  

	// Check if user supplied with custom free function
	// if so use it, else just use free
	if(ht->usr_free!=NULL)
		ht->usr_free(p->value);
	else
		free(p->value);

	// Free the key
	free(p->key);

	// Nullify the two values
	p->key = NULL;
	p->value = NULL;

	// dec the nmem field
	ht->nmem--;

	// return success
	return 0;
}

void ht_free(HashTable * ht){
	
	// loop over the table and free each value and key
	// then free the pair itself
	for(int i=0; i<ht->length; i++){
		Pair * p = &ht->arr[i];

		// If the pair is empty continue
		if(p->key==NULL){
			continue;
		}

		// If the pair is not empty free the key and value
		free(p->key);

		// since the value can have nested values it might
		// need freeing check if the user has provided a 
		// custom free function as a callback and if so use it
		// otherwise simply free the value

		if(ht->usr_free==NULL){
			free(p->value);
			continue;
		}else{
			ht->usr_free(p->value);
		}
		
	}

	// free the array holding the table entries
	free(ht->arr);

	// free the table itself
	free(ht);

}

struct test{
	char * str;
};

void cfree(void * value){
	char * str = ((struct test *) value)->str;
	free(str);
	free(value);
}

int main(void){

	int size = 20;
	HashTable * ht = ht_create(size);
	int nmem = 3;

	char * key;
	//char * value;
	struct test * value;
	for(int i=0; i<nmem; i++){
		key = malloc(32);
		//value = malloc(32);
		value = malloc(sizeof(struct test));
		value->str = malloc(32);
		sprintf(key, "key %d", i);
		sprintf(value->str, "value %d", i);
		ht_insert(ht, key, value);
	}

	//char ** keys = ht_get_keys(ht);
	//ht_delete(ht, "key 0");
	//ht_delete(ht, "key 1");
	//ht_delete(ht, "key 2");
	//ht_delete(ht, "key 2");

	//clock_t start = clock();
	for(int i=0; i<nmem; i++){
		//char * key = keys[i];
		key = malloc(32);
		sprintf(key, "key %d", i);
		Pair * pair  =  ht_get_pair(ht, key);
		if(value==NULL) printf("no such key\n");
		else printf("%s : %s\n", key, ((struct test *) pair->value)->str);
		free(key);
	}

	ht_set_free(ht, cfree);
	ht_free(ht);


}
