#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#define JSON_BUF_SIZE 2048
#define FILENAME_BUF_SIZE 256
#define SEARCH_BUF_SIZE 128
#define DEF_BUF_SIZE 128

int allocate(char **str, int size) {
  char *temp;
  temp = realloc(*str, sizeof(char) * size);
  // temp = malloc(sizeof(char) * size);
  if (!temp) {
    printf("\nCouldn't allocate memory!\n\n");
    return 0;
  }
  *str = temp;
  return 1;
}

char *prompt_search(char *buffer) {
  printf("Search.\n\tEnter a word to search for:\n> ");

  while (1) {
    scanf(" %127[^\n]", buffer);
    // scanf("%s", buffer);
    if (buffer != NULL) {
      break;
    }
    printf("Wrong input! Try again...\n> ");
    fflush(stdin);
  }
}

void search_for_word(char *filename) {
  if (!filename) {
    printf("\nNo dictionary selected! Select it and try again...\n\n");
    return;
  }

  char *search_buffer = NULL;
  if (!allocate(&search_buffer, SEARCH_BUF_SIZE))
    return;

  prompt_search(search_buffer);

  FILE *fp;
  char *json_buffer = NULL;
  if (!allocate(&json_buffer, JSON_BUF_SIZE))
    return;

  fp = fopen(filename, "r");
  if (fp) {
    fread(json_buffer, JSON_BUF_SIZE, 1, fp);
    fclose(fp);
  } else {
    printf("Couldn't open file! Try again later...");
    return;
  }

  json_object *json = json_tokener_parse(json_buffer);
  free(json_buffer);

  int found = 0;
  json_object *subarray, *element;
  for (int i = 0; i < json_object_array_length(json); i++) {
    subarray = json_object_array_get_idx(json, i);
    element = json_object_array_get_idx(subarray, 0);

    if (!strcmp(json_object_get_string(element), search_buffer)) {
      found = 1;
      break;
    }
  }

  if (found) {
    json_object *reses, *res, *name, *pos, *def;

    reses = json_object_array_get_idx(subarray, 1);
    
    for (int k = 0; k < json_object_array_length(reses); k++) {
      res = json_object_array_get_idx(reses, k);
      name = json_object_array_get_idx(res, 0);
      pos = json_object_array_get_idx(res, 1);
      def = json_object_array_get_idx(res, 2);

      printf("\t%i. %s\n", k + 1, json_object_get_string(name));
      printf("\t   %s\n", json_object_get_string(pos));
      printf("\t   %s\n\n", json_object_get_string(def));
    }
  } else {
    printf("\nCouldn't find the word...\n\n> ");
  }
}

void get_insert_info(char *out, char *pos, char *def) {
  printf("\tEnter the word in target language:\n> ");
  scanf(" %[^\n]", out);
  printf("\tEnter part of speech (you can leave it blank):\n> ");
  scanf(" %[^\n]", pos);
  printf("\tEnter the definition:\n> ");
  scanf(" %[^\n]", def);
}

void insert_word(char *filename) {
  if (!filename) {
    printf("\nNo dictionary selected! Add it and try again...\n\n");
    return;
  }

  FILE *fp;
  char *json_buffer = NULL;
  if (!allocate(&json_buffer, JSON_BUF_SIZE))
    return;

  fp = fopen(filename, "r");
  if (fp) {
    fread(json_buffer, JSON_BUF_SIZE, 1, fp);
    fclose(fp);
  } else {
    printf("Couldn't open file! Try again later...");
    return;
  }

  json_object *json = json_tokener_parse(json_buffer);

  if (!json_object_is_type(json, json_type_array)) {
    json_object_put(json);
    json = json_object_new_array();
  }

  int *size = malloc(sizeof(int));
  char *inp = NULL, *out = NULL, *pos = NULL, *def = NULL;
  if (!allocate(&inp, DEF_BUF_SIZE) || !allocate(&out, DEF_BUF_SIZE) || !allocate(&pos, DEF_BUF_SIZE) || !allocate(&def, DEF_BUF_SIZE))
    return;

  while (1) {
    printf("Insert.\n\tEnter the word:\n> ");
    fflush(stdin);
    if (!scanf(" %[^\n]", inp)) {
      printf("\nWrong input! Try again...\n\n> ");
      continue;
    }
    while (1) {
      printf("\tEnter how many definitions will the word have:\n> ");
      fflush(stdin);
      if (!scanf("%i", size)) {
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
  

  for (int i = 0; i < *size; i++) {
    json_object *defarray = json_object_new_array();

    get_insert_info(out, pos, def);
    
    json_object_array_add(defarray, json_object_new_string(out));
    json_object_array_add(defarray, json_object_new_string(pos));
    json_object_array_add(defarray, json_object_new_string(def));
    json_object_array_add(subarray, defarray);
  }
  json_object_array_add(arr, subarray);
  json_object_array_add(json, arr);

  fp = fopen(filename, "w");
  if (fp && json) {
    fprintf(fp, "%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    fclose(fp);
  } else {
    printf("Couldn't open dictionary or read it...\n");
  }

  json_object_put(json);

  printf("\nWord %s was added successfully!\n\n> ", inp);
}

void change_dictionary(char **filename) {
  if (!allocate(filename, FILENAME_BUF_SIZE))
    return;

  if (!*filename) {
    printf("\nNot enough memory to open a file...\n\n> ");
    return;
  }

  printf("Change dictionary.\n\tEnter filename:\n> ");

  while (1) {
    fflush(stdin);
    scanf("%s", *filename);
    if (*filename != NULL) {
        FILE *fp;
        fp = fopen(*filename, "r");
        if (!fp) {
          printf("No such file! Try again...\n> ");
          continue;
        }
    } else {
      printf("Wrong input! Try again...\n> ");
      continue;
    }

    char *dot = strrchr(*filename, '.');
    if (dot) {
      if (!strcmp(".json", dot)) {
        printf("\nDictionary %s was successfully opened!\n\n> ", *filename);
        break;
      } else {
        printf("\nWrong file extension! Dictionary has to be .json\n\n> ");
      }
    } else {
      printf("\nWrong file extension! Dictionary has to be .json\n\n> ");
    }
  }
}

int main() {
  char *menu_buffer = malloc(sizeof(char) * 50);
  char *filename = NULL;

  while(1) {

    printf("To proceed select an option:\n");
    printf("\t1. Search in the dictionary\n");
    printf("\t2. Insert a word into the dictionary\n");
    printf("\t3. Select dictionary\n");
    printf("\t4. Exit\n\n> ");

    scanf(" %c", menu_buffer);

    switch(menu_buffer[0]) {
      case '1':
        search_for_word(filename);
        break;
      case '2':
        insert_word(filename);
        break;
      case '3':
        change_dictionary(&filename);
        break;
      case '4':
        printf("\nGoodbye!\n\n");
        return 0;
      default:
        printf("\nWrong input! Try again...\n\n");
        break;
    }

    fflush(stdin);
  }
}