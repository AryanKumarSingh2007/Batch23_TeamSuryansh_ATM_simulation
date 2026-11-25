/*
 * main.c - integration: menus, login/logout flow, saving
 * Flow: load accounts -> welcome menu -> login -> ATM menu -> logout
 * The accounts array lives in main() and is passed to other functions.
 */

#include <stdio.h>
#include "atm.h"

/* static = used only inside this file */
static void showWelcomeMenu(void);
static void showAtmMenu(void);
static void saveOrWarn(const Account accounts[], int count);
static void runAtmSession(Account accounts[], int count, int index);

int main(void)
{
    Account accounts[MAX_ACCOUNTS];
    int accountCount;
    int choice;
    int index;
    int running = 1;

    /* Load accounts (loadAccounts prints the exact problem if it fails) */
    accountCount = loadAccounts(accounts, MAX_ACCOUNTS);
    if (accountCount < 0)
    {
        printf("The ATM cannot start without a valid account file.\n");
        return 1;
    }
    if (accountCount == 0)
    {
        printf("Error: '%s' contains no accounts. The ATM cannot start.\n",
               ACCOUNTS_FILE);
        return 1;
    }

    printf("Loaded %d account(s) from %s.\n", accountCount, ACCOUNTS_FILE);

    while (running)
    {
        showWelcomeMenu();
        if (!readInt("Enter your choice: ", &choice))
        {
            choice = 0;                 /* bad input = invalid choice */
        }

        switch (choice)
        {
            case 1:
                index = login(accounts, accountCount);
                if (index == -1)
                {
                    /* 3 wrong attempts: program ends */
                    printf("Your session has been locked. Please restart the program to try again.\n");
                    running = 0;
                }
                else
                {
                    runAtmSession(accounts, accountCount, index);
                }
                break;

            case 2:
                printf("\nThank you for using the ATM Simulation. Goodbye!\n");
                running = 0;
                break;

            default:
                printf("\nInvalid choice. Please enter 1 or 2.\n");
                break;
        }
    }

    return 0;
}

static void showWelcomeMenu(void)
{
    printf("\n=============================\n");
    printf("   WELCOME TO THE ATM\n");
    printf("=============================\n");
    printf("1. Login\n");
    printf("2. Exit\n");
}

static void showAtmMenu(void)
{
    printf("\n=============================\n");
    printf("         ATM MENU\n");
    printf("=============================\n");
    printf("1. Check Balance\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Mini Statement\n");
    printf("5. Change PIN\n");
    printf("6. Logout\n");
}

/* Saves all accounts; warns if saving fails */
static void saveOrWarn(const Account accounts[], int count)
{
    if (!saveAccounts(accounts, count))
    {
        printf("Warning: could not save changes to '%s'. Changes may be lost!\n",
               ACCOUNTS_FILE);
    }
}

/* Menu loop for one logged-in user. We pass &accounts[index] (a pointer)
 * so the other functions change the real account. */
static void runAtmSession(Account accounts[], int count, int index)
{
    Account *current = &accounts[index];
    int loggedIn = 1;
    int choice;

    while (loggedIn)
    {
        showAtmMenu();
        if (!readInt("Enter your choice: ", &choice))
        {
            choice = 0;
        }

        switch (choice)
        {
            case 1:
                checkBalance(current);
                break;

            case 2:
                if (deposit(current))           /* 1 = balance changed, so save */
                {
                    saveOrWarn(accounts, count);
                }
                break;

            case 3:
                if (withdraw(current))
                {
                    saveOrWarn(accounts, count);
                }
                break;

            case 4:
                miniStatement(current);
                break;

            case 5:
                if (changePin(current))
                {
                    saveOrWarn(accounts, count);
                }
                break;

            case 6:
                printf("\nYou have been logged out. Thank you for using the ATM!\n");
                loggedIn = 0;
                break;

            default:
                printf("\nInvalid choice. Please enter a number from 1 to 6.\n");
                break;
        }
    }
}
