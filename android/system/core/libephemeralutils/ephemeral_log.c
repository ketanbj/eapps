#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ephemeralutils/ephemeral_log.h>
#include <sqlite3.h>

//#define EPHEMERAL_DB "file:///data/ephemeralapps/ephemeral.db"
#define EPHEMERAL_DB "ephemeral.db"

static int logcb(void *NotUsed, int argc, char **argv, char **azColName){
   	int i;
   	for(i=0; i<argc; i++){
      		LOGPI("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   	}
   	LOGPI("\n");
   	return 0;
}

int ephemeral_log(const char* appname,ephemeral_log_type type,const      char *logpath)
{

	sqlite3 *db;
   	char *zErrMsg = 0;
   	int  rc;
   	char *sql = (char*) malloc(1024*sizeof(char));
	/* Open database */
   	rc = sqlite3_open(EPHEMERAL_DB, &db);
   	if( rc ){
      		LOGPI("Can't open database: %s\n", sqlite3_errmsg(db));
      		return -1;
   	}else{
      		LOGPI("Opened database successfully\n");
   	}

	/* create table for app if it ddoesn't exists & log */
	sprintf(sql,"CREATE TABLE IF NOT EXISTS [%s](path TEXT PRIMARY KEY, type INTEGER);INSERT INTO [%s](Path,Type) VALUES('%s',%d);",appname, appname,logpath,type);
	LOGPI("SQL Querry: %s",sql);
	rc = sqlite3_exec(db, sql, logcb, 0, &zErrMsg);
   	if( rc != SQLITE_OK ){
   		LOGPI("SQL error: %s\n", zErrMsg);
      		sqlite3_free(zErrMsg);
   	}else{
      		LOGPI("Table created successfully\n");
   	}

	/* close database */
	sqlite3_close(db);
	return 0;
}

static int truncatecb(void *data, int argc, char **argv, char **azColName){
	int i;
   	LOGPI("%s: ", (const char*)data);
   	for(i=0; i<argc; i++){
      		LOGPI("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   	}
   	LOGPI("\n");
  	//FIXME: perform removal of logged state here
   	return 0;
}

int ephemeral_truncate(const char* appname)
{
	sqlite3 *db;
   	char *zErrMsg = 0;
   	int rc;
   	const char* data = "Truncate callback function called";
   	char *sql = (char*) malloc(1024*sizeof(char));

	/* Open database */
        rc = sqlite3_open(EPHEMERAL_DB, &db);
        if( rc ){
                LOGPI("Can't open database: %s\n", sqlite3_errmsg(db));
                return -1;
        }else{
                LOGPI("Opened database successfully\n");
        }

	sprintf(sql, "SELECT * FROM [%s];",appname);
	LOGPI("SQL Querry: %s",sql);
	/* Execute SQL statement */
   	rc = sqlite3_exec(db, sql, truncatecb, (void*)data, &zErrMsg);
   	if( rc != SQLITE_OK ){
      		LOGPI("SQL error: %s\n", zErrMsg);
      	sqlite3_free(zErrMsg);
   	}else{
      		LOGPI("Operation done successfully\n");
   	}
	/* close database */
        sqlite3_close(db);
	return 0;
}

int ephemeral_clearlog(const char* appname)
{
	sqlite3 *db;
        char *zErrMsg = 0;
        int rc;
        const char* data = "Clear callback function called";
        char *sql = (char*) malloc(1024*sizeof(char));

        /* Open database */
        rc = sqlite3_open(EPHEMERAL_DB, &db);
        if( rc ){
                LOGPI("Can't open database: %s\n", sqlite3_errmsg(db));
                return -1;
        }else{
                LOGPI("Opened database successfully\n");
        }

        sprintf(sql, "DROP TABLE IF EXISTS [%s];",appname);
	LOGPI("SQL Querry: %s",sql);
        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, truncatecb, (void*)data, &zErrMsg);
        if( rc != SQLITE_OK ){
                LOGPI("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        }else{
                LOGPI("Operation done successfully\n");
        }
        /* close database */
        sqlite3_close(db);
        return 0;

}
