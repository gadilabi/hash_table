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

