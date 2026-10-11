#include "gui.h"
#include "functions.h"
#include <QApplication>

// Much of my program was coded by itself, using visual studio code. I then switched to using
// Qt Creator. Because of this, I kept some imports that I could have done otherways
// using Qt, particularly for outputting data to the console.


int main(int argc, char *argv[])
{
    //encryption crypt;
    bool terminal = false;
    bool fail = false;
    if (argc > 1) {
        for(int opt = 1;opt<argc;opt++) {
            if (argv[opt][2] == '\0' && argv[opt][0] == '-') {
                switch (argv[opt][1]){
                default:
                    printf("Error: Unknown arg %s.\n", argv[opt]);
                    return 1;
                case 't':
                    terminal = true;
                    break;
                case 'h':
                    std::cout << "Available commands:\n\n"
                                 "-h\tProvide help\n"
                                 "-t\tActivate terminal mode\n\n"
                                 "Or leave blank in order to run GUI\n";
                    return 0;
                    break;

                }
            }
        }
    }
    if (terminal == true)
    {
        login login;
        Database db;
        std::string auth;
        while (terminal == true)
        {
            while (auth != "r" && auth != "l")
            {
                std::cout << "Do you want to register or login? (r/l)\n";
                try
                {
                    std::cin >> auth;
                }
                catch (int error)
                {
                    std::cout << "Error code: " << error << "\n";
                    std::cout << "\nProbably invalid input\n";
                }
            }
            unsigned char username[128] = {0};
            unsigned char password[128] = {0};
            int c = 0;
            std::string tempStorage;
            while (login.hasAuthenticated() == false)
            {
                try
                {
                    std::cout << "Press enter on blank field to exit\n\n"
                                 "Enter your username: \n";
                    // Code here was copied from stackoverflow:
                    // https://stackoverflow.com/questions/2907062/fgets-instructions-gets-skipped-why

                    /* discard all characters up to and including newline */
                    c = 0;
                    while ((c = getchar()) != '\n' && c != EOF)
                        ;

                    fgets((char *)username, 128, stdin);
                    std::cout << "Enter your password: \n";
                    fgets((char *)password, 128, stdin);
                    username[strlen((char *)username) - 1] = '\0';
                    password[strlen((char *)password) - 1] = '\0';
                    // fgets adds on the final \n character, this above just removes it
                    if (username[0] == '\0' || password[0] == '\0')
                    {
                        return 0;
                    }
                    else
                    {
                        if (auth == "r")
                        {
                            c = login.registerUser((unsigned char *)username, (unsigned char *)password, &db);
                            if (c == 1)
                            {
                                std::cout << "Error registering: \n"
                                             "Username must be 5 characters or greater\n"
                                             "Password must be 8 characters or greater\n"
                                             "Press enter to try another username, password\n"
                                             "Or type l then press enter to enter login mode.\n";
                                std::getline(std::cin, tempStorage);
                            }
                            else if (c == 0)
                            {
                                break;
                            }
                        }
                        else if (auth == "l")
                        {
                            c = login.authenticate(username, password, &db);
                            if (c == 1)
                            {
                                std::cout << "Error authenticating: \n"
                                             "Database with that username and password may not exist\n"
                                             "Press enter to try another username, password\n"
                                             "Or type r then press enter to enter registration mode.\n";
                                std::getline(std::cin, tempStorage);
                            }
                            else if (c == 0)
                            {
                                break;
                            }
                        }
                        if (tempStorage == "l")
                        {
                            auth = "l";
                            tempStorage = "";
                        }
                        else if (tempStorage == "r")
                        {
                            auth = "r";
                            tempStorage = "";
                        }
                        *password = '\0';
                        *username = '\0';
                    }
                }
                catch (int error)
                {
                    *password = '\0';
                    *username = '\0';
                }
            }
            std::cout << "\nAuthenticated!\n\nPress Enter to continue:\n";
            try
            {
                std::getline(std::cin, tempStorage);
            }
            catch (int error)
            {
                ;
            }
            tempStorage = "";
            bool quit = false;
            int id = 0;
            char *passwordStorage = new char[128];
            char *usernameStorage = new char[128];
            char *websiteStorage = new char[128];

            // Allows me to store as much data as I want in memory without having to worry about limits.
            std::vector<std::vector<std::string> > results;
            while (quit == false)
            {
                try
                {
                    std::cout << "What would you like to do?\n\n"
                                 "(d) Delete stored account by ID\n"
                                 "(i) Insert new account information\n"
                                 "(s) Show database\n"
                                 "(u) Update account by ID\n"
                                 "Or press enter to exit.\n";
                    std::getline(std::cin, tempStorage);
                    if (tempStorage == "")
                    {
                        quit = true;
                        return 0;
                    }

                    if (tempStorage == "d")
                    {
                        tempStorage = "";
                        while (true)
                        {
                            try
                            {
                                std::cout << "Press enter to return to the previous menu\n"
                                             "Enter the ID of the account to delete\n";
                                std::cin >> id;
                                if (db.deleteRecord(id) != 0)
                                {
                                    std::cout << "ID does not exist\n";
                                }
                            }
                            catch (int error)
                            {
                                break;
                            }
                        }
                    }
                    if (tempStorage == "i")
                    {
                        while (true)
                        {
                            try
                            {
                                c = 0;
                                while ((c = getchar()) != '\n' && c != EOF);
                                std::cout << "Hold Enter to exit\n";
                                std::cout << "Enter the password you want to store.\nPassword must be 8 characters or more.\n";

                                fgets(passwordStorage, 128, stdin);
                                passwordStorage[strlen((char *)passwordStorage) - 1] = '\0';
                                std::cout << "Enter the username you want to store.\nEither Username or Website must be 5 characters or more.\n";
                                c = 0;
                                //while ((c = getchar()) != '\n' && c != EOF);
                                fgets(usernameStorage, 128, stdin);
                                usernameStorage[strlen((char *)usernameStorage) - 1] = '\0';
                                std::cout << "Enter the website you want to store.\nEither Website or Username must be 5 characters or more.\n";
                                c = 0;
                                //while ((c = getchar()) != '\n' && c != EOF);
                                fgets(websiteStorage, 128, stdin);
                                websiteStorage[strlen(websiteStorage) - 1] = '\0';
                                if (passwordStorage[0] != '\0' || (usernameStorage[0] != '\0' && websiteStorage[0] != '\0'))
                                {
                                    int error = db.insert((const char *)passwordStorage, login.shaPassword, (const char *)usernameStorage, (const char *)websiteStorage);
                                    if (error != 0)
                                    {
                                        std::cout << "Success!\n Press enter to continue.\n";
                                        std::cin;
                                    }
                                    std::cout << error << "\n";
                                }
                                else
                                {
                                    break;
                                }
                            }
                            catch (int error)
                            {
                                break;
                            }
                        }
                        delete[] usernameStorage;
                        delete[] passwordStorage;
                        delete[] websiteStorage;
                    }
                    if (tempStorage == "s")
                    {
                        try {
                            tempStorage = "";
                            int error = db.select(login.shaPassword, results);
                            if (error == 0) {
                                for (int i = 0; i < results.size(); i++) {
                                    for (int j = 0; j < 4; j++) {
                                        //if (!results[i][j].empty()) {
                                        std::cout << results[i][j];
                                        if (j < 3) {
                                            std::cout << " | ";
                                            //}
                                        }
                                    }
                                    std::cout << std::endl;
                                }
                            }
                            else
                            {
                                std::cout << "Error: " << error << "\n";
                            }
                        }
                        catch (const std::exception& ex) {
                            std::cerr << "\nException caught: " << ex.what() << std::endl;
                            std::cout << "An exception occurred during the select operation\n";
                        }
                    }

                    if (tempStorage == "u")
                    {
                        tempStorage = "";
                        char scanStore[2];
                        int ID = 0;
                        char updatePassword[128];
                        char updateUsername[128];
                        char updateWebsite[128];
                        while (true)
                        {
                            try
                            {
                                printf("Enter 0 or nothing to exit\nEnter the ID of the details to update: ");
                                char input[128];
                                fgets(input, sizeof(input), stdin);
                                if (sscanf(input, " %d", &ID) != 1) {
                                    break;
                                }
                                printf("Enter nothing to exit, 'p' to update the password, 'u' to update the username, or 'w' to update the website: ");
                                fgets(input, sizeof(input), stdin);
                                if (sscanf(input, " %s", &scanStore) != 1) {
                                    break;
                                }
                                if (ID != 0 && strlen(scanStore) > 0)
                                    switch (scanStore[0])
                                    {
                                    case 'p': // Update the password
                                        printf("Enter the new password: ");
                                        scanf("%s", updatePassword);
                                        db.update(ID, pass, login.shaPassword, updatePassword);
                                        break;
                                    case 'u': // Update the username
                                        printf("Enter the new username: ");
                                        scanf("%s", updateUsername);
                                        db.update(ID, user, login.shaPassword, "", updateUsername);
                                        break;
                                    case 'w': // Update the website
                                        printf("Enter the new website: ");
                                        scanf("%s", updateWebsite);
                                        db.update(ID, site, login.shaPassword, "", "", updateWebsite);
                                        break;
                                    default:
                                        printf("Invalid input.\nType 'e' to exit\n");
                                        scanf("%s", scanStore);
                                        if (scanStore[0]=='e') {break;}
                                    }
                                if (ID == 0 || scanStore[0] == '\0') {break;}
                            }
                            catch (int error)
                            {
                                break;
                            }
                        }
                    }
                }
                catch (const std::exception& ex) {
                    std::cerr << "Exception caught: " << ex.what() << std::endl;
                }
            }
        }
        return 0;
    }
    else {
        QApplication a(argc, argv);
        passwordManager w;
        w.show();
        return a.exec();
    }
}
