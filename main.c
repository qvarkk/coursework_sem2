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
    printf("\t   %s\n", json_object_get_string(def));
  }
}

int main() {
  char menu_buffer;
  while(1) {
    printf("To proceed select an option:\n");
    printf("\t1. Search in the dictionary\n");
    printf("\t2. Insert a word into the dictionary\n");
    printf("\t3. Exit\n> ");

    scanf(" %c", &menu_buffer);

    switch(menu_buffer) {
      case '1':
        search_for_word();
        break;
      case '2':
        break;
      case '3':
        printf("\nGoodbye!\n\n");
        return 0;
        break;
    }

    fflush(stdin);
  }
}