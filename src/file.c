#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

// Struct to store downloaded file data
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

int main() {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Will be grown as needed by realloc
    chunk.size = 0;             // No data at this point

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(curl_handle) {
        // Set URL to download
        curl_easy_setopt(curl_handle, CURLOPT_URL, "https://file-examples.com/index.php/sample-documents-download/sample-odt-download/");

        // Set callback function to write data into memory
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request
        res = curl_easy_perform(curl_handle);

        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            printf("%lu bytes received\n", (unsigned long)chunk.size);
            printf("Downloaded content:\n%s\n", chunk.memory);
        }

        // Clean up curl
        curl_easy_cleanup(curl_handle);
    }

    // Clean up memory
    free(chunk.memory);
    curl_global_cleanup();

    return 0;
}

