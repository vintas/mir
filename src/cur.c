#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

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
void http_get_request(const char *url) {
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
        }
        else {
            // JSON parsing with json-c
            struct json_object *root = json_tokener_parse(chunk.memory);
            if(root == NULL) {
                fprintf(stderr, "Error parsing JSON\n");
            }
            else {
                printf("GET Request Response:\n%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
                json_object_put(root);  // Free JSON object
            }
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();
}

// Function to perform HTTP POST request with JSON data
void http_post_request(const char *url, const char *data) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Will be grown as needed by realloc
    chunk.size = 0;             // No data at this point

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            // JSON parsing with json-c
            struct json_object *root = json_tokener_parse(chunk.memory);
            if(root == NULL) {
                fprintf(stderr, "Error parsing JSON\n");
            }
            else {
                printf("POST Request Response:\n%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
                json_object_put(root);  // Free JSON object
            }
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();
}

int main() {
    const char *get_url = "https://jsonplaceholder.typicode.com/posts";
    //const char *get_url = "https://4bb57fc2-219e-421e-86b4-4ffda6bf1b3b.mock.pstmn.io/matches";
    const char *post_url = "https://jsonplaceholder.typicode.com/posts";
    const char *post_data = "{\"title\":\"foo\",\"body\":\"bar\",\"userId\":1}";

    printf("Performing GET request...\n");
    http_get_request(get_url);

    printf("\nPerforming POST request...\n");
    http_post_request(post_url, post_data);

    return 0;
}

