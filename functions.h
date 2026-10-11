#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstring>   // Allows use of strlen()
#include <string>    // Allows me to use the data type string
#include <vector>    // Allows me to use Vectors, used in my select function
#include <sqlite3.h> // Allows me to use SQL in C++

#include <openssl/aes.h>
#include <openssl/evp.h>

#include <cstdio>
#include <iostream>
// Basic import for outputting text to the console
// Because C++ is object oriented, nearly all code is in a function
// The function main is run automatically on startup, all others are not

class encryption
{
private:
    std::hash<unsigned char> hash_basic;

public:
    // Encrypt plaintext using the given key using AES in ECB mode
    // Returns the encrypted ciphertext
    char *encrypt(unsigned char *plaintext, unsigned char *key);

    // Decrypt ciphertext using the given key using AES in ECB mode
    // Assumes the ciphertext is in the form of an array of hexadecimal numbers
    // Returns the decrypted plaintext
    unsigned char* decrypt(char* ciphertext, unsigned char* key);


    // Sha256 algorithm, made possible by OpenSSL library.
    // Digest should be a unsigned char[] array, with length 64
    void sha256(unsigned char *message, unsigned char *digest);

    // Fowler–Noll–Vo hash function, except I use the standard
    // hash algorithm, in order to try and improve security
    unsigned long long int hash(unsigned char *message);
};

// Enumerate creates a new data type that only allows preset values, Used here for only allowing specific keywords
enum keyword
{
    pass,
    user,
    site
};

// The class for all the functions and variables to do with the database.
class Database
{

private:
    // The filename of the database.
    const char *filename;

    // Creating a table if the database does not already have one.
    int createTable(sqlite3 *database);

    // Address/Variable to contain SQL errors.
    char *errormsg;
    encryption crypt;

public:
    // The pointer to the database.
    sqlite3 *database;
    static int callback(void *data, int argc, char **argv, char **azColName);

    // Simple function to check whether there was an error with the SQL operation.
    char *errorCheck(int rc);

    // Inserts data into the database.
    int insert(const char *password, unsigned char *key, const char *username = "", const char *website = "");

    // Opens the database, must be called first.
    // Slightly weird way of doing it, but sqlite3_open takes a const char * as an input
    // So in order for me to use an input of a string later on, I have to cast it to a const char *
    // Hopefully I can do this in a better way later, however this will do for now
    int open(char file[], bool createNew = false);

    // Select a record from the table
    int select(unsigned char *key, std::vector<std::vector<std::string> >& results, bool displayResult = true, const char* where="");


    // Updates a record from the table, however it can only be updated one cell at a time currently.
    int update(int ID, keyword keyword, unsigned char *key, const char *password = "", const char *username = "", const char *website = "");

    // Deletes a record from the table by ID, very simple.
    int deleteRecord(int ID);
};

class login
{
private:
    encryption crypt;
    bool authenticated = false;
public:
    unsigned char shaPassword[64] = {0};
    bool hasAuthenticated();
    int logout();
    int registerUser(unsigned char *username, unsigned char *password, Database *db);
    int authenticate(unsigned char *username, unsigned char *password, Database *db);
};

#endif
