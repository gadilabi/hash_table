#define HT_HASH_PRIME 31
#define HT_RESIZE_THRESHOLD 0.2
#define HT_RESIZE_FACTOR 2
#define HT_INSERT_UPDATE 1
#define HT_INSERT_EMPTY 0

// typedefs
typedef struct ht HashTable;
typedef struct key_value Pair;

// function declarations
void * ht_get_pair(HashTable * ht, char * key);
void * ht_get_value(HashTable * ht, char * key);
char ** ht_get_keys(HashTable * ht);
int ht_delete(HashTable * ht, char * key);
int ht_insert(struct ht * ht , char * key, void * value);
struct ht * ht_create(int length);
void ht_set_free(struct ht * ht, void (* usr_free)(void * garbage_value));
void ht_free(HashTable * ht);

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

