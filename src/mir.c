#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>  // SQLite library header

#define MAX_CMD_LEN 258
#define CREATE_DEP_TABLE "CREATE TABLE DEPENDENCY(ID INT PRIMARY KEY NOT NULL, RESOURCE_ID INT, DEPENDENCY_ID INT);"
#define INSERT_INTO_DEP "INSERT INTO DEPENDENCY (ID,RESOURCE_ID,DEPENDENCY_ID) VALUES (%d,%d,%d);"
#define LIST_LIBS "find /lib /usr/lib -type f -o -type l -exec ls {} +"
#define LIST_BIN "find /bin /usr/bin /data -type f  -o -type l -executable"
#define READ_DEP "./readelf -d  %s%s >> /tmp/Chetan"
int dep_count=1;
int r_id;
sqlite3 *db;
char *zErrMsg = 0;
int count = 1;


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int get_id(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++) {
      r_id = atoi(argv[0]);
      printf("ID = %d",atoi(argv[0]));
    }
    return 0;
}
void separatePathAndFilename(const char *path, char *directory, char *filename) {
    char *lastSlash = strrchr(path, '/');  // Find the last occurrence of '/'
    
    if (lastSlash != NULL) {
        // Found a slash, split path into directory and filename
        strncpy(directory, path, lastSlash - path + 1);  // Copy directory part
        directory[lastSlash - path + 1] = '\0';          // Null-terminate directory string
        strcpy(filename, ++lastSlash);                 // Copy filename part
        filename[strlen(filename)-1] = '\0';
    } else {
        // No slash found, assume the whole input is the filename
        strcpy(directory, "");
        strncpy(filename, path,strlen(path)-1);
    }
}

// Function to handle SQLite errors
static int relate_resources(void *NotUsed, int argc, char **argv, char **azColName)
{
  char cmd[128]={0};
  int resource_id = 0;
  char file[128] = {0};
  char path[128] = {0};
  char buffer[1024] = {0};
  FILE *fp;
  char get_id_query[128] = {0};
  if (argc == 2) {
    // Construct full file path
    char full_path[256]; // Adjust size according to your needs
    snprintf(full_path, sizeof(full_path), "%s%s", argv[0], argv[1]);
    strcpy(path, argv[0]);
    strncpy(file, argv[1],strlen(argv[1]));
    snprintf(cmd, sizeof(full_path), "./readelf -d %s%s | grep NEEDED",path,file );
    sprintf(get_id_query, "SELECT ID FROM 'RESOURCE' WHERE NAME = '%s';",file);

    sqlite3_exec(db, get_id_query, get_id, 0, &zErrMsg);
    resource_id = r_id;

    printf("cmd is : %s\n",cmd);
    fp = popen(cmd, "r");
    if (fp == NULL) {
      perror("Failed to run readelf command");
      return 0;
    }
    // Print full file path
    printf("File Path: %s\n", full_path);
  } else {
    fprintf(stderr, "Unexpected number of columns returned by query!\n");
    return 0;
  }


  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("\n\n\n %s",buffer);
    const char result[128] = {0}; // Find first occurrence of '['
    const char *start = strchr(buffer, '['); // Find first occurrence of '['
    const char *end = strchr(buffer, ']');   // Find first occurrence of ']'

    if (start && end && end > start) {
      size_t length = end - start - 1;
      int dependency_id = 0;
      char *result = malloc(length + 1);
      if (result != NULL) {
        strncpy(result, start + 1, length);
        result[length] = '\0'; // Null-terminate the string

        // Print the extracted substring
        printf("Extracted substring: %s\n", result);
        sprintf(get_id_query, "SELECT ID FROM 'RESOURCE' WHERE NAME = '%s';",result);
        sqlite3_exec(db, get_id_query, get_id, 0, &zErrMsg);
        dependency_id = r_id;
        memset(cmd,0,128);
        sprintf(cmd, INSERT_INTO_DEP, dep_count++,resource_id,dependency_id);
        printf("res = %d dep = %d \n\n\n\n\n",resource_id,dependency_id);
        sqlite3_exec(db, cmd, callback, 0, &zErrMsg);


        // Free allocated memory
        free(result);
      } else {
        fprintf(stderr, "Memory allocation failed!\n");
      }
    } else {
      printf("No substring enclosed in brackets found.\n");
    }
  }
  pclose(fp);

  return 0;

  /*   const char *start = strchr(buffer, '['); // Find first '['
       if(!start) continue;
       if (start)
       {
       printf("\n\n\n cmd output %s",buffer );

       start++; // Move past '['
       const char *end = strchr(start, ']'); // Find matching ']'
       if (end)  
       {   
       int length = end - start; // Calculate length of substring
       strncpy(outputString, start, length); // Copy substring to output
       outputString[length] = '\0'; // Null-terminate output string

       sqlite3_exec(db,"SELECT ID FROM RESOURCE WHERE NAME ", get_id, 0, &zErrMsg);

       printf("dependency id  %d",dependency_id);
       printf("%s", argv[i] ? argv[i] : "NULL");
       }
       } 
       }
  }
  return 1;*/
}

int update_lib()
{
    FILE *fp;
    char buffer[1024];
    char sql_insert[256] = {0};
    char file_dir[256] = {0};
    char file_name[54] = {0};

    // Open the command for reading
    fp = popen(LIST_LIBS, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        return 1;
    }
    // SQL statement to create a table
    const char *sql = "CREATE TABLE RESOURCE("
                      "ID INT PRIMARY KEY     NOT NULL,"
                      "NAME  TEXT    NOT NULL,"
                      "VERSION TEXT,"
                      "VER_FORMAT TEXT,"
                      "PATH TEXT NOT NULL,"
                      "DELETED INT );";
    
    // Execute SQL statement
    sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    sqlite3_exec(db, CREATE_DEP_TABLE, callback, 0, &zErrMsg);

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        //printf("%s", buffer);
        separatePathAndFilename(buffer, file_dir, file_name);
        printf("dir : %s\n", file_dir);
        printf("file : %s\n", file_name);
        sprintf(sql_insert,"INSERT INTO RESOURCE (ID,NAME,VERSION,VER_FORMAT,PATH,DELETED) VALUES (%d ,'%s','nil','nil','%s',0);",count,file_name,file_dir);
        count++; 
    // Execute SQL statement
    sqlite3_exec(db, sql_insert, callback, 0, &zErrMsg);
    memset(sql_insert,0,256); 
    memset(file_dir,0,256); 
    memset(file_name,0,54); 
    }

    // Close the pipe and print any error
    if (pclose(fp) != 0) {
        perror("Command not found or exited with error");
        return 1;
    }
    return 0;
}
void update_bin()
{
    FILE *fp;
    char buffer[1024];
    char sql_insert[256] = {0};
    char file_dir[256] = {0};
    char file_name[54] = {0};
    
    // Open the command for reading
    fp = popen(LIST_BIN, "r");
    if (fp == NULL) {
        perror("Failed to run command");
        return ;
    }

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        //printf("%s", buffer);
        separatePathAndFilename(buffer, file_dir, file_name);
        printf("dir : %s\n", file_dir);
        printf("file : %s\n", file_name);
        sprintf(sql_insert,"INSERT INTO RESOURCE (ID,NAME,VERSION,VER_FORMAT,PATH,DELETED) VALUES (%d ,'%s','nil','nil','%s',0);",count,file_name,file_dir);
        count++; 
    // Execute SQL statement
    sqlite3_exec(db, sql_insert, callback, 0, &zErrMsg);
    memset(sql_insert,0,256); 
    memset(file_dir,0,256); 
    memset(file_name,0,54); 
    }

    // Close the pipe and print any error
    if (pclose(fp) != 0) {
        perror("Command not found or exited with error");
        return;
    }
    return;
}


int main(int argc, char *argv[]) {
    printf("%s %s",argv[1],argv[2]);
    int rc;
    char list_lib_cmd[MAX_CMD_LEN] = "SELECT COUNT(*) FROM RESOURCE;";
    char list_bin_cmd[MAX_CMD_LEN] = "SELECT COUNT(*) FROM RESOURCE;";

    // Open database connection (creates if not exists)
    rc = sqlite3_open("/data/test.db", &db);
    
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    //check is the table empty
    rc = sqlite3_exec(db, list_lib_cmd, callback, 0, &zErrMsg);
    update_lib();
    rc= sqlite3_exec(db, list_bin_cmd, callback, 0, &zErrMsg);
    update_bin();
    
    rc = sqlite3_exec(db,"SELECT PATH,NAME FROM RESOURCE", callback, 0, &zErrMsg);
    rc = sqlite3_exec(db,"SELECT PATH,NAME FROM RESOURCE", relate_resources, 0, &zErrMsg);
    
    
    //handling the request

    switch(argc)
    { 
      case 3 : printf("case 3");
               char install_cmd[108] = {0};
               if(strcmp(argv[1],"list") == 0)
               {
                  if(strcmp(argv[2],"lib") == 0)
                  {
                       const char *sql_select = "SELECT * from COMPANY";
                       printf("DB entries \n\n\n\n\n %s",sql_select);
                       
                       // Execute SQL statement
                       rc = sqlite3_exec(db, sql_select, callback, 0, &zErrMsg);
                       
                       if (rc != SQLITE_OK) {
                           fprintf(stderr, "SQL error: %s\n", zErrMsg);
                           sqlite3_free(zErrMsg);
                       } else {
                           fprintf(stdout, "Operation done successfully\n");
                       }

                  }
                  else if(strcmp(argv[2],"bin") == 0)
                  {

                  }
                  
               }
               else if(strcmp(argv[1],"install") == 0)
               {
                  sprintf(install_cmd,"mir-get %s",argv[2]);
                  printf("%s",install_cmd);
                  system(install_cmd);
               }
               break;
      default:printf("default");
              break;
    }
    
  /*  // SQL statement to create a table
    const char *sql = "CREATE TABLE COMPANY("
                      "ID INT PRIMARY KEY     NOT NULL,"
                      "NAME           TEXT    NOT NULL,"
                      "AGE   `         INT     NOT NULL,"
                      "ADDRESS        CHAR(50),"
                      "SALARY         REAL );";
    
    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
    
    // SQL statement to insert data
    const char *sql_insert = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
                             "VALUES (1, 'Paul', 32, 'California', 20000.00 ); "
                             "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
                             "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "
                             "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
                             "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"
                             "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
                             "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
    
    // Execute SQL statement
    rc = sqlite3_exec(db, sql_insert, callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }
    // SQL statement to select data
    const char *sql_select = "SELECT * from COMPANY";
    printf("DB entries \n\n\n\n\n %s",sql_select);
    
    // Execute SQL statement
    rc = sqlite3_exec(db, sql_select, callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    
    */
    // Close database connection
    sqlite3_close(db);
    
    return 0;
}

