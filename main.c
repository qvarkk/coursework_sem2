#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

char search_buffer[128];
char *prompt_search() {
  printf("Search.\n\tEnter a word to search for:\n> ");

  while (1) {
    scanf("%s", search_buffer);
    if (search_buffer != NULL) {
      break;
    }
    printf("Wrong input! Try again...\n> ");
    fflush(stdin);
  }

  return search_buffer;
}

void search_for_word() {
  char *prompt_buffer = prompt_search();

  FILE *fp;
  char json_buffer[2048];

  fp = fopen("term_bank_1.json", "r");
  fread(json_buffer, 2048, 1, fp);
  fclose(fp);

  json_object *json = json_tokener_parse(json_buffer);

  int found;
  json_object *subarray, *element;
  for (int i = 0; i < json_object_array_length(json); i++) {
    subarray = json_object_array_get_idx(json, i);
    element = json_object_array_get_idx(subarray, 0);

    if (!strcmp(json_object_get_string(element), prompt_buffer)) {
      break;
    }
  }

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
}

void get_insert_info(char *out, char *pos, char *def) {
  printf("\tEnter the word in target language:\n> ");
  scanf("%s", out);
  printf("\tEnter part of speech (you can leave it blank):\n> ");
  scanf("%s", pos);
  printf("\tEnter the definition:\n> ");
  scanf("%s", def);
}

void insert_word() {
  FILE *fp;
  char json_buffer[2048];

  fp = fopen("term.json", "r");
  fread(json_buffer, 2048, 1, fp);
  fclose(fp);

  json_object *json = json_tokener_parse(json_buffer);

  if (!json_object_is_type(json, json_type_array)) {
    json_object_put(json);
    json = json_object_new_array();
  }

  int *size = malloc(sizeof(int));
  char *inp = malloc(sizeof(char) * 128), *out = malloc(sizeof(char) * 128), *pos = malloc(sizeof(char) * 128), *def = malloc(sizeof(char) * 128);
  
  printf("Insert.\n\tEnter the word:\n> ");
  scanf("%s", inp);
  printf("\tEnter how many definitions will the word have:\n> ");
  scanf("%i", size);

  for (int i = 0; i < *size; i++) {
    json_object *subarray = json_object_new_array();
    json_object *arr = json_object_new_array();

    get_insert_info(out, pos, def);
    
    json_object_array_add(arr, json_object_new_string(inp));
    json_object_array_add(subarray, json_object_new_string(out));
    json_object_array_add(subarray, json_object_new_string(pos));
    json_object_array_add(subarray, json_object_new_string(def));
    json_object_array_add(arr, subarray);
    json_object_array_add(json, arr);
  }

  fp = fopen("term.json", "w");
  if (fp != NULL && json != NULL) {
    fprintf(fp, "%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    fclose(fp);
  } else {
    printf("Meow\n");
  }

  json_object_put(json);
}

int main() {
  char menu_buffer;
  while(1) {
    printf("To proceed select an option:\n");
    printf("\t1. Search in the dictionary\n");
    printf("\t2. Insert a word into the dictionary\n");
    printf("\t3. Exit\n\n> ");

    scanf(" %c", &menu_buffer);

    switch(menu_buffer) {
      case '1':
        search_for_word();
        break;
      case '2':
        insert_word();
        break;
      case '3':
        printf("\nGoodbye!\n\n");
        return 0;
    }

    fflush(stdin);
  }
}