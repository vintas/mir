#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "stack.h"

// Struct to store HTTP response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to write received data into memory
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        // Out of memory
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to perform HTTP GET request and parse JSON response
void http_get_request(const char *url, Stack *stack) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Will be grown as needed by realloc
    chunk.size = 0;             // No data at this point

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // JSON parsing with json-c
            struct json_object *root = json_tokener_parse(chunk.memory);
            if(root == NULL) {
                fprintf(stderr, "Error parsing JSON\n");
            } else {
                printf("GET Request Response:\n%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
                
                struct json_object *resource_location_obj;
                if (json_object_object_get_ex(root, "resource_location", &resource_location_obj)) {
                    const char *resource_location = json_object_get_string(resource_location_obj);
                    push(stack, resource_location);
                }

                struct json_object *dependency_array;
                if (json_object_object_get_ex(root, "dependency", &dependency_array)) {
                    int array_len = json_object_array_length(dependency_array);
                    for (int i = 0; i < array_len; i++) {
                        struct json_object *dependency_obj = json_object_array_get_idx(dependency_array, i);
                        struct json_object *dname_obj;
                        struct json_object *ver_obj;
                        if (json_object_object_get_ex(dependency_obj, "dname", &dname_obj) && 
                            json_object_object_get_ex(dependency_obj, "ver", &ver_obj)) {
                            // Construct the new URL for the dependency
                            char new_url[256];
                            // snprintf(new_url, sizeof(new_url), "https://your-api-endpoint/%s/%s", 
                            //          json_object_get_string(dname_obj), 
                            //          json_object_get_string(ver_obj));
                            // Recursive call to get the dependency
                            snprintf(new_url, sizeof(new_url), "http://localhost:8000/%s/%s", 
                            json_object_get_string(dname_obj), 
                            json_object_get_string(ver_obj));

                            http_get_request(new_url, stack);
                        }
                    }
                }
                json_object_put(root);  // Free JSON object
            }
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();
}

// Main function to demonstrate HTTP GET request and store resource locations in a stack
int main(int argc, char *argv[]) {
    Stack stack;
    initialize(&stack);
    const char *get_url = "http://localhost:8000/openvpn/1.2.3";

    // const char *get_url = "https://your-api-endpoint/openvpn/1.2.3";
    http_get_request(get_url, &stack);

    printf("Resource Locations:\n");
    while (!isEmpty(&stack)) {
        char *resource_location = pop(&stack);
        printf("%s\n", resource_location);
        free(resource_location);
    }

    return 0;
}
