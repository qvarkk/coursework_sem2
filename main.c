#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#define FILENAME_BUF_SIZE 256
#define SEARCH_BUF_SIZE 128
#define DEF_BUF_SIZE 128

typedef struct BSTNode {
  const char *word;
  int index;
  struct BSTNode *left;
  struct BSTNode *right;
} BSTNode;

BSTNode* createBSTNode(const char *word, int index) {
  BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
  newNode->word = word;
  newNode->index = index;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

BSTNode* insertBSTNode(BSTNode *root, const char *word, int index) {
  if (root == NULL) {
    return createBSTNode(word, index);
  }

  if (strcmp(word, root->word) < 0) {
    root->left = insertBSTNode(root->left, word, index);
  } else {
    root->right = insertBSTNode(root->right, word, index);
  }

  return root;
}

int searchBSTNode(BSTNode *root, const char *word) {
  if (root == NULL || strcmp(word, root->word) == 0) {
    return (root == NULL) ? -1 : root->index;
  }

  if (strcmp(word, root->word) < 0) {
    return searchBSTNode(root->left, word);
  } else {
    return searchBSTNode(root->right, word);
  }
}

int allocate(char **str, int size)
{
  char *temp;
  temp = realloc(*str, sizeof(char) * size);
  if (!temp)
  {
    printf("\nCouldn't allocate memory!\n\n");
    return 0;
  }
  *str = temp;
  return 1;
}

char *prompt_search(char *buffer)
{
  printf("Search.\n\tEnter a word to search for:\n> ");

  while (1)
  {
    scanf(" %127[^\n]", buffer);
    if (buffer != NULL)
    {
      break;
    }
    printf("Wrong input! Try again...\n> ");
    fflush(stdin);
  }
}

void search_for_word_file(FILE *file, long int *filesize)
{
  if (!file)
  {
    printf("\nNo dictionary selected! Select it and try again...\n\n");
    return;
  }

  char *search_buffer = NULL;
  if (!allocate(&search_buffer, SEARCH_BUF_SIZE))
    return;

  prompt_search(search_buffer);

  char *json_buffer = NULL;
  if (!allocate(&json_buffer, *filesize))
    return;

  fread(json_buffer, *filesize, 1, file);
  fseek(file, 0, SEEK_SET);

  json_object *json = json_tokener_parse(json_buffer);
  // free(json_buffer);

  int found = 0;
  json_object *subarray, *element;
  for (int i = 0; i < json_object_array_length(json); i++)
  {
    subarray = json_object_array_get_idx(json, i);
    element = json_object_array_get_idx(subarray, 0);

    if (strstr(json_object_get_string(element), search_buffer))
    {
      found = 1;
      break;
    }
  }

  if (found)
  {
    json_object *reses, *res, *name, *pos, *def;

    reses = json_object_array_get_idx(subarray, 1);

    for (int k = 0; k < json_object_array_length(reses); k++)
    {
      res = json_object_array_get_idx(reses, k);
      name = json_object_array_get_idx(res, 0);
      pos = json_object_array_get_idx(res, 1);
      def = json_object_array_get_idx(res, 2);

      printf("\t%i. %s\n", k + 1, json_object_get_string(name));
      printf("\t   %s\n", json_object_get_string(pos));
      printf("\t   %s\n\n", json_object_get_string(def));
    }
  }
  else
  {
    printf("\nCouldn't find the word...\n\n> ");
  }
}

void BSTSearch(FILE *file, long int *filesize) {
  if (!file)
  {
    printf("\nNo dictionary selected! Select it and try again...\n\n");
    return;
  }

  char *search_buffer = NULL;
  if (!allocate(&search_buffer, SEARCH_BUF_SIZE))
    return;

  prompt_search(search_buffer);

  char *json_buffer = NULL;
  if (!allocate(&json_buffer, *filesize))
    return;

  fread(json_buffer, *filesize, 1, file);
  fseek(file, 0, SEEK_SET);

  json_object *json = json_tokener_parse(json_buffer);
  json_object *json_array = json_object_get(json);

  if (json_array != NULL) {
    int array_length = json_object_array_length(json_array);
 
    BSTNode *root = NULL;

    for (int i = 0; i < array_length; i++) {
      json_object *entry = json_object_array_get_idx(json_array, i);
      const char *word = json_object_get_string(json_object_array_get_idx(entry, 0));
      root = insertBSTNode(root, word, i);
    }

    int index = searchBSTNode(root, search_buffer);

    if (index != -1)
    {
      json_object *subarray = json_object_array_get_idx(json, index);
      json_object *reses, *res, *name, *pos, *def;

      reses = json_object_array_get_idx(subarray, 1);

      for (int k = 0; k < json_object_array_length(reses); k++)
      {
        res = json_object_array_get_idx(reses, k);
        name = json_object_array_get_idx(res, 0);
        pos = json_object_array_get_idx(res, 1);
        def = json_object_array_get_idx(res, 2);

        printf("\t%i. %s\n", k + 1, json_object_get_string(name));
        printf("\t   %s\n", json_object_get_string(pos));
        printf("\t   %s\n\n", json_object_get_string(def));
      }
    } else {
      printf("No such word found!\n\n> ");
    }

    json_object_put(json_array);
    free(root);
  }
}

void get_insert_info(char *out, char *pos, char *def)
{
  printf("\tEnter the word in target language:\n> ");
  scanf(" %[^\n]", out);
  printf("\tEnter part of speech (you can leave it blank):\n> ");
  scanf(" %[^\n]", pos);
  printf("\tEnter the definition:\n> ");
  scanf(" %[^\n]", def);
}

void insert_word_file(FILE **fp, long int **filesize)
{
  if (!*fp)
  {
    printf("\nNo dictionary selected! Add it and try again...\n\n");
    return;
  }

  char *json_buffer = NULL;
  if (!allocate(&json_buffer, **filesize))
    return;

  fread(json_buffer, **filesize, 1, *fp);
  fseek(*fp, 0, SEEK_SET);

  json_object *json = json_tokener_parse(json_buffer);

  if (!json_object_is_type(json, json_type_array))
  {
    json_object_put(json);
    json = json_object_new_array();
  }

  int *size = malloc(sizeof(int));
  char *inp = NULL, *out = NULL, *pos = NULL, *def = NULL;
  if (!allocate(&inp, DEF_BUF_SIZE) || !allocate(&out, DEF_BUF_SIZE) || !allocate(&pos, DEF_BUF_SIZE) || !allocate(&def, DEF_BUF_SIZE))
    return;

  while (1)
  {
    printf("Insert.\n\tEnter the word:\n> ");
    fflush(stdin);
    if (!scanf(" %[^\n]", inp))
    {
      printf("\nWrong input! Try again...\n\n> ");
      continue;
    }
    while (1)
    {
      printf("\tEnter how many definitions will the word have:\n> ");
      fflush(stdin);
      if (!scanf("%i", size))
      {
        printf("\nWrong input! Try again...\n\n> ");
        continue;
      }
      break;
    }
    break;
  }

  json_object *arr = json_object_new_array();
  json_object *subarray = json_object_new_array();
  json_object_array_add(arr, json_object_new_string(inp));

  for (int i = 0; i < *size; i++)
  {
    json_object *defarray = json_object_new_array();

    get_insert_info(out, pos, def);

    json_object_array_add(defarray, json_object_new_string(out));
    json_object_array_add(defarray, json_object_new_string(pos));
    json_object_array_add(defarray, json_object_new_string(def));
    json_object_array_add(subarray, defarray);
  }
  json_object_array_add(arr, subarray);
  json_object_array_add(json, arr);

  if (json)
  {
    fprintf(*fp, "%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
  }
  else
  {
    printf("Couldn't open dictionary or read it...\n");
  }

  fseek(*fp, 0, SEEK_END);
  **filesize = ftell(*fp);
  fseek(*fp, 0, SEEK_SET);
  printf("%ld", **filesize);

  json_object_put(json);

  printf("\nWord %s was added successfully!\n\n> ", inp);
}

void change_dictionary_file(FILE **fp, long int **filesize)
{
  char *filename = NULL;

  if (!allocate(&filename, FILENAME_BUF_SIZE))
    return;

  if (!filename)
  {
    printf("\nNot enough memory to open a file...\n\n> ");
    return;
  }

  printf("Change dictionary.\n\tEnter filename (or x to exit):\n> ");

  while (1)
  {
    fflush(stdin);
    scanf("%s", filename);
    if (filename[0] == 'x') {
      return;
    }
    if (filename != NULL)
    {
      *fp = fopen(filename, "r+");
      if (!*fp)
      {
        printf("No such file! Try again...\n> ");
        continue;
      }
    }
    else
    {
      printf("Wrong input! Try again...\n> ");
      continue;
    }

    char *dot = strrchr(filename, '.');
    if (dot)
    {
      if (!strcmp(".json", dot))
      {
        printf("\nDictionary %s was successfully opened!\n\n", filename);
        break;
      }
      else
      {
        printf("\nWrong file extension! Dictionary has to be .json\n\n> ");
      }
    }
    else
    {
      printf("\nWrong file extension! Dictionary has to be .json\n\n> ");
    }
  }

  fseek(*fp, 0, SEEK_END);
  **filesize = ftell(*fp);
  fseek(*fp, 0, SEEK_SET);

  free(filename);
}

int main()
{
  char *menu_buffer = malloc(sizeof(char) * 50);
  FILE *file = NULL;
  long int *filesize = malloc(sizeof(int));

  while (1)
  {

    printf("To proceed select an option:\n");
    printf("\t1. Search in the dictionary (loop)\n");
    printf("\t2. Search in the dictionary (BST, sometimes it doesn't work)\n");
    printf("\t3. Insert a word into the dictionary\n");
    printf("\t4. Select dictionary\n");
    printf("\t5. Exit\n\n> ");

    scanf(" %c", menu_buffer);

    switch (menu_buffer[0])
    {
    case '1':
      search_for_word_file(file, filesize);
      break;
    case '2':
      BSTSearch(file, filesize);
      break;
    case '3':
      insert_word_file(&file, &filesize);
      break;
    case '4':
      change_dictionary_file(&file, &filesize);
      break;
    case '5':
      printf("\nGoodbye!\n\n");
      return 0;
    default:
      printf("\nWrong input! Try again...\n\n");
      break;
    }

    fflush(stdin);
  }
}