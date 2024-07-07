#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>  // SQLite library header

#define MAX_CMD_LEN 256

sqlite3 *db;
char *zErrMsg = 0;

// Function to handle SQLite errors
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int update_lib()
{
    FILE *fp;
    char buffer[1024];
    int count = 0;

    // Open the command for reading
    fp = popen("ls /usr/bin/", "r");
    if (fp == NULL) {
        perror("Failed to run command");
        return 1;
    }
    // SQL statement to create a table
    const char *sql = "CREATE TABLE COMPANY("
                      "ID INT PRIMARY KEY     NOT NULL,"
                      "LIB_NAME  TEXT    NOT NULL);";
    
    // Execute SQL statement
    sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    // Read the output line by line
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        //printf("%s", buffer);
        char sql_insert[256] = {0};
        sprintf(sql_insert,"INSERT INTO COMPANY (ID,LIB_NAME) VALUES (%d ,'%s');",count,buffer);
        count++; 
    // Execute SQL statement
    sqlite3_exec(db, sql_insert, callback, 0, &zErrMsg);
    memset(sql_insert,0,256); 
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
    return;
}


int main(int argc, char *argv[]) {
    printf("%s %s",argv[1],argv[2]);
    int rc;
    char list_lib_cmd[MAX_CMD_LEN] = "SELECT COUNT(*) FROM lib_list;";
    char list_bin_cmd[MAX_CMD_LEN] = "SELECT COUNT(*) FROM bin_list;";

    // Open database connection (creates if not exists)
    rc = sqlite3_open("test.db", &db);
    
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

