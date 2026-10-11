#include "functions.h"

char* encryption::encrypt(unsigned char *plaintext, unsigned char *key)
{
    unsigned char *ciphertext = new unsigned char[AES_BLOCK_SIZE];
    // unsigned char iv[AES_BLOCK_SIZE] = {0};

    AES_KEY aes_key;
    AES_set_encrypt_key(key, 128, &aes_key);
    AES_encrypt(plaintext, ciphertext, &aes_key);

    // Convert the ciphertext to a hex string
    char *hex_ciphertext = new char[2 * AES_BLOCK_SIZE + 1];
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
    {
        sprintf((hex_ciphertext + 2 * i), "%02x", ciphertext[i]);
    }
    hex_ciphertext[2 * AES_BLOCK_SIZE] = '\0';

    delete[] ciphertext;
    return hex_ciphertext;
};

unsigned char* encryption::decrypt(char* ciphertext, unsigned char* key) {
    unsigned char* plaintext = new unsigned char[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    // Convert the ciphertext from a hex string to a byte array
    unsigned char ciphertext_bytes[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        sscanf((ciphertext + 2*i), "%02x", &ciphertext_bytes[i]);
    }

    AES_KEY aes_key;
    AES_set_decrypt_key(key, 128, &aes_key);
    AES_decrypt(ciphertext_bytes, plaintext, &aes_key);

    return plaintext;
}

void encryption::sha256(unsigned char *message, unsigned char *digest)
{
    // Create a new SHA-256 context
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    // Initialize the context for SHA-256 hashing
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

    // Update the context with the message data, and finding the length of the message to do so
    EVP_DigestUpdate(ctx, message, strlen((char *)message));

    // Finalize the hash and store it in the digest buffer
    EVP_DigestFinal_ex(ctx, digest, NULL);

    // Clean up the context
    EVP_MD_CTX_free(ctx);
};

unsigned long long int encryption::hash(unsigned char *message)
{
    // AKA FNV offset basis
    unsigned long long int hashed = 253927217;
    unsigned long long int prime = 2901573757;
    for (size_t i = 0; i < strlen((char *)message); i++)
    {
        hashed = (hashed ^ hash_basic(message[i]));
        hashed = hashed * prime;
    }
    return hashed;
};

int Database::createTable(sqlite3 *database)
{
    const char *SQL_TABLE;
    SQL_TABLE = "create TABLE DATA ("
                "ID INTEGER PRIMARY KEY,"
                "WEBSITE   TEXT,"
                "USERNAME  TEXT,"
                "PASSWORD  TEXT);";
    int rc = sqlite3_exec(database, SQL_TABLE, callback, 0, &errormsg);
    return rc;
};

int Database::callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char *)data);

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
};

char* Database::errorCheck(int rc)
{
    if (rc != SQLITE_OK)
    {
        char *fin = 0;
        snprintf(fin, 200, "SQL Error: %s\n", errormsg);
        errormsg = 0;
        std::cout << fin;
        return fin;
    }
    return 0;
};

int Database::insert(const char *password, unsigned char *key, const char *username, const char *website)
{
    if ((strlen(username) >= 5 || strlen(website) >= 5) && strlen(password) >= 8)
    {
        char SQL_INSERT[601];
        snprintf(SQL_INSERT, 600,
                    "INSERT INTO DATA (WEBSITE,USERNAME,PASSWORD) VALUES ('%s', '%s', '%s');",
                    crypt.encrypt((unsigned char *)website, key),
                    crypt.encrypt((unsigned char *)username, key),
                    crypt.encrypt((unsigned char *)password, key)); // Creates command and adds result to SQL_INSERT
        // executing command, SQL_INSERT must be of type (const char*), so I cast it to const char
        int rc = sqlite3_exec(database, (const char *)SQL_INSERT, callback, 0, &errormsg);
        *SQL_INSERT = 0;
        website = 0;
        username = 0;
        password = 0;
        // Deleting data from memory
        return rc;
    }
    website = 0;
    username = 0;
    password = 0;
    // errormsg = 0;
    return 1;
};

int Database::open(char file[], bool createNew)
{
    errormsg = 0;
    // Filenames have to be given as a const char format,
    char tempStorage[strlen(file) + 4]; // allocate enough space for the string and ".db" extension
    strcpy(tempStorage, file);
    // strcat would add .db onto the string twice
    strcat(tempStorage, ".db");
    const char *filename = tempStorage;

    if (createNew == false)
    {
        int exit = sqlite3_open_v2(filename, &database, 0x00000002, NULL); // Only opens the database
        if (exit)
        {
            exit = 0;
            return 1;
        }
        return 0;
    } // Slightly confusing, if (exit) means if (exit != 0), which only happens when there's an error,
    // So if (exit) {return 1;} means 'If theres an error, return 1'. Normally a function returns 0.
    int exit = sqlite3_open(filename, &database); // Opens and creates the database
    createTable(database);
    if (exit)
    {
        exit = 0;
        return 1;
    }
    return 0;
};

int Database::select(unsigned char *key, std::vector<std::vector<std::string> >& results, bool displayResult, const char* where)
{
    results.clear();
    const char *SQL_SELECT = "SELECT ID, WEBSITE, USERNAME, PASSWORD FROM DATA";
    if (where != "") {sprintf((char*)SQL_SELECT, "%s WHERE %s;", SQL_SELECT, where);}
    int rc;

    sqlite3_stmt *statement;
    rc = sqlite3_prepare_v2(database, SQL_SELECT, -1, &statement, nullptr);
    if (rc != SQLITE_OK)
    {
        return rc;
    }
    if (displayResult == true) {
        results.push_back(std::vector<std::string>());
        results.back().push_back("ID");
        results.back().push_back("Website");
        results.back().push_back("Username");
        results.back().push_back("Password");
    }
    int id = 0;
    std::string decrypted_website = "";
    std::string decrypted_username = "";
    std::string decrypted_password = "";
    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        id = sqlite3_column_int(statement, 0);
        // Get the length of the text column and a pointer to its data
        char* website_data = (char*)(sqlite3_column_text(statement, 1));

        // Decrypt the text and create a string
        decrypted_website = (std::string)((const char*)(crypt.decrypt(website_data, key)));

        // Repeat for the other columns
        char* username_data = (char*)(sqlite3_column_text(statement, 2));
        decrypted_username = (std::string)((const char*)(crypt.decrypt(username_data, key)));

        char* password_data = (char*)(sqlite3_column_text(statement, 3));
        decrypted_password = (std::string)((const char*)(crypt.decrypt(password_data, key)));

        results.push_back(std::vector<std::string>());
        results.back().push_back(std::to_string(id));
        results.back().push_back(decrypted_website);
        results.back().push_back(decrypted_username);
        results.back().push_back(decrypted_password);
    }
    id = 0;
    decrypted_website = "";
    decrypted_username = "";
    decrypted_password = "";
    sqlite3_finalize(statement);
    return 0;
}


int Database::update(int ID, keyword keyword, unsigned char *key, const char *password, const char *username, const char *website)
{
    if (strlen(username) > 5 || strlen(website) > 5 || strlen(password) > 8)
    {
        const char *set = 0;
        const char *setTo = 0;
        switch (keyword)
        { // Using an enumeration to check the correct data to update
        case pass:
            set = "PASSWORD";
            setTo = password;
            break;
        case user:
            set = "USERNAME";
            setTo = username;
            break;
        case site:
            set = "WEBSITE";
            setTo = website;
            break;
        }
        sqlite3_busy_timeout(database, 10000);
        char SQL_UPDATE[601]; // Aside from this, It's quite basic except it uses a WHERE ID = number command
        char *encryptedSetTo = crypt.encrypt((unsigned char *)setTo, key);
        snprintf(SQL_UPDATE, 600, "UPDATE DATA set %s = '%s' WHERE ID = %d;", set, (const char *)encryptedSetTo, ID);
        int rc = sqlite3_exec(database, (const char *)SQL_UPDATE, callback, 0, &errormsg);
        if (rc != SQLITE_OK) {
            printf("Error message: %s\n", errormsg);
        }
        if (rc == 5) {std::cout << "Other program(s) are accessing the database, and %s cannot be updated.\nTry closing proccesses that may be accessing the database\n", set;}
        *SQL_UPDATE = 0;
        password = 0;
        username = 0;
        website = 0;
        ID = 0;
        keyword = site;
        // I keep doing this to attempt to delete and remove all the data about the password/username
        // from memory, however I haven't attempted to find them in memory, so it could not be working right now in theory
        // I have read on stackoverflow it works, however it is possible I should be using delete[].
        return rc;
    }
    password = 0;
    username = 0;
    website = 0;
    ID = 0;
    keyword = site;
    return 1;
};

int Database::deleteRecord(int ID)
{
    if (ID > 0)
    {
        char del[101]; // Very simple, just using what we've learnt before about SQLite commands
        snprintf(del, 100, "DELETE from DATA where ID = %d;", ID);
        int rc = sqlite3_exec(database, (const char *)del, callback, 0, &errormsg);
        return rc;
    }
    return 1;
};

bool login::hasAuthenticated() { return authenticated; }

int login::logout()
{
    // shaUsername[64] = {0};
    memset(shaPassword, 0, sizeof(shaPassword));
    authenticated = false;
    return 0;
};

int login::registerUser(unsigned char *username, unsigned char *password, Database *db)
{
    logout();
    if (strlen((char *)username) >= 5 && (strlen((char *)password) >= 8))
    {
        // Hashing and saving the hashed password,username to the class
        // In order for logging in.
        unsigned long long int hashedUsername = crypt.hash(username);
        unsigned long long int hashedPassword = crypt.hash(password);

        char file[128] = "";
        snprintf(file, 128, "%llu:%llu", hashedUsername, hashedPassword);
        if (db->open(file, false) != 0)
        {
            db->open(file, true);
            // Hashing password in order to use as a key later
            // crypt.sha256(username, shaUsername);
            crypt.sha256(password, shaPassword);
            username = 0;
            password = 0;
            authenticated = true;
            return 0;
        }
        hashedPassword = 0;
        hashedUsername = 0;
        username = 0;
        password = 0;
        return 1;
    }
    username = 0;
    password = 0;
    return 1;
};

int login::authenticate(unsigned char *username, unsigned char *password, Database *db)
{
    logout();
    if (strlen((char *)username) >= 5 && (strlen((char *)password) >= 8))
    {
        // Hashing and saving the hashed password,username to the class
        // In order for logging in.
        unsigned long long int hashedUsername = crypt.hash(username);
        unsigned long long int hashedPassword = crypt.hash(password);

        char file[128] = "";
        snprintf(file, 128, "%llu:%llu", hashedUsername, hashedPassword);
        if (db->open(file, false) == 0)
        {
            // Hashing password in order to use as a key later
            // crypt.sha256(username, shaUsername);
            crypt.sha256(password, shaPassword);
            username = 0;
            password = 0;
            authenticated = true;
            return 0;
        }
    }
    username = 0;
    password = 0;
    return 1;
};
