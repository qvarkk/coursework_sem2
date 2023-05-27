#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

// Structure for BST node
typedef struct BSTNode {
    const char *word;
    int index;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

// Function to create a new BST node
BSTNode* createBSTNode(const char *word, int index) {
    BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->word = word;
    newNode->index = index;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Function to insert a node into the BST
BSTNode* insertBSTNode(BSTNode *root, const char *word, int index) {
    // Base case: Empty tree, create a new node
    if (root == NULL) {
        return createBSTNode(word, index);
    }

    // Recursive insertion
    if (strcmp(word, root->word) < 0) {
        root->left = insertBSTNode(root->left, word, index);
    } else {
        root->right = insertBSTNode(root->right, word, index);
    }

    return root;
}

// Function to search for a word in the BST and return its index
int searchBSTNode(BSTNode *root, const char *word) {
    // Base cases: Empty tree or word found at the current node
    if (root == NULL || strcmp(word, root->word) == 0) {
        return (root == NULL) ? -1 : root->index;
    }

    // Recursive search
    if (strcmp(word, root->word) < 0) {
        return searchBSTNode(root->left, word);
    } else {
        return searchBSTNode(root->right, word);
    }
}

int main() {
    char *search_word = malloc(128 * sizeof(char));
    scanf(" %128[^\n]", search_word);

    char filename[50] = "term_bank_1.json";
    FILE *fp;
    char *json_str = malloc(2048 * sizeof(char));

    fp = fopen(filename, "r");
    if (fp)
    {
      fread(json_str, 2048, 1, fp);
      fclose(fp);
    }
    else
    {
      printf("Couldn't open file! Try again later...");
      return 1;
    }

    // Parse the JSON string
    json_object *json = json_tokener_parse(json_str);

    // Get the JSON array
    json_object *json_array = NULL;
    if (json_object_is_type(json, json_type_array)) {
        json_array = json_object_get(json);
    }

    // Check if the JSON array exists
    if (json_array != NULL) {
        int array_length = json_object_array_length(json_array);

        // Create a Binary Search Tree (BST) from the JSON array
        BSTNode *root = NULL;

        for (int i = 0; i < array_length; i++) {
            json_object *entry = json_object_array_get_idx(json_array, i);
            const char *word = json_object_get_string(json_object_array_get_idx(entry, 0));
            root = insertBSTNode(root, word, i);
        }

        // Search for a word in the BST
        int index = searchBSTNode(root, search_word);

        if (index != -1) {
            printf("The word '%s' is found at index %d\n", search_word, index);
        } else {
            printf("The word '%s' is not found in the JSON array\n", search_word);
        }

        // Clean up
        json_object_put(json_array);
        free(root);
    }

    // Clean up the JSON object
    if (json != NULL) {
        json_object_put(json);
    }

    return 0;
}