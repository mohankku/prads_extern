#ifndef __REDIS_LIB_H
#define __REDIS_LIB_H

#include "../deps/hiredis/hiredis.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#define ASSET_HASH4(ip) ((ip) % BUCKET_SIZE)
#define BUCKET_SIZE  31337

typedef int (*get_key_val)(void *, char *);
typedef int (*put_key_val) (char *, void *);
typedef uint32_t (*key_hash) (void *key);

typedef struct item_t {
  	struct item_t *next;
  	struct item_t *prev;
  	uint64_t cas;
  	void*    key;
  	size_t   nkey;
  	void*    data;
  	size_t   size;
  	uint32_t flags;
  	time_t   exp;
  	pthread_mutex_t mutex;
} item;

typedef struct redis_client_t {
	redisContext 	*context;
	item 		*passet[BUCKET_SIZE];
	char 		*key_type;
	size_t		 key_size;
	get_key_val	 get;
	put_key_val	 put;
	key_hash         hash;
	uint32_t	 flags;
	uint32_t	 exp;
	// hash
} redis_client;

redis_client client;

// Policy should be update with this cache
redis_client *create_cache(char *host, int port);

void destroy_cache(redis_client *client);

// create item, hash item and update local cache with the data 
int create_item(void* key, size_t nkey, void *data,
		  size_t size, uint32_t flags, time_t exp);

int free_item(void* key, size_t nkey);

// returns the client context after setting up the connection
redisContext *createClient(char *host, int port);

// Syncronous Get and Set methods.
int redis_syncSet(redisContext *c, char *key, int key_len, char *value, int value_len);
redisReply* redis_syncGet(redisContext *c, char *key, size_t key_len);

// Syncronous Get and Set methods.
int redis_asyncSet(char *key, int key_len, char *value, int value_len);
int redis_asyncGet(char *key, int key_len, char *value, int *value_len);

int destroyClient(redisContext *context);

int register_encode_decode(get_key_val, put_key_val, key_hash);

#endif