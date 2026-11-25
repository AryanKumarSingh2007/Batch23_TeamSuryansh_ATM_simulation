/*
 * storage.c - file handling and safe input (Member 1)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "atm.h"

#define INPUT_SIZE 64   /* longest input line we read */

/* ---------- Account file ---------- */

/* Reads accounts.txt (one line per account: number pin balance).
 * Returns the number of accounts, or -1 on any problem. */
int loadAccounts(Account accounts[], int maxAccounts)
{
    FILE *fp = fopen(ACCOUNTS_FILE, "r");
    Account temp;
    int count = 0;
    int result;

    if (fp == NULL)
    {
        printf("Error: cannot open '%s'.\n", ACCOUNTS_FILE);
        printf("Make sure the file exists in the same folder as the program.\n");
        return -1;
    }

    /* fscanf returns 3 when all three values were read */
    while ((result = fscanf(fp, "%d %d %lf",
                            &temp.accountNumber, &temp.pin, &temp.balance)) == 3)
    {
        if (count == maxAccounts)
        {
            printf("Error: '%s' has more than %d accounts.\n",
                   ACCOUNTS_FILE, maxAccounts);
            fclose(fp);
            return -1;
        }

        /* "!(x >= 0)" also rejects NaN */
        if (temp.accountNumber <= 0 || temp.pin < MIN_PIN ||
            temp.pin > MAX_PIN || !(temp.balance >= 0))
        {
            printf("Error: account record %d in '%s' has invalid data.\n",
                   count + 1, ACCOUNTS_FILE);
            fclose(fp);
            return -1;
        }

        if (findAccount(accounts, count, temp.accountNumber) != -1)
        {
            printf("Error: account number %d appears twice in '%s'.\n",
                   temp.accountNumber, ACCOUNTS_FILE);
            fclose(fp);
            return -1;
        }

        accounts[count] = temp;
        count++;
    }

    /* Loop ended: either end of file (fine) or bad data (error) */
    if (result != EOF)
    {
        printf("Error: '%s' is not formatted correctly after account record %d.\n",
               ACCOUNTS_FILE, count);
        printf("Each line must be:  accountNumber pin balance\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return count;
}

/* Overwrites accounts.txt with the whole array. Returns 1 on success. */
int saveAccounts(const Account accounts[], int count)
{
    FILE *fp = fopen(ACCOUNTS_FILE, "w");
    int i;

    if (fp == NULL)
    {
        return 0;
    }

    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%d %d %.2f\n",
                accounts[i].accountNumber, accounts[i].pin, accounts[i].balance);
    }

    if (fclose(fp) != 0)
    {
        return 0;
    }
    return 1;
}

/* Linear search. Returns index, or -1 if not found. */
int findAccount(const Account accounts[], int count, int accountNumber)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (accounts[i].accountNumber == accountNumber)
        {
            return i;
        }
    }
    return -1;
}

/* ---------- Transaction history ---------- */

/* Appends one line: accountNumber TYPE amount balanceAfter */
int appendTransaction(int accountNumber, const char *type,
                      double amount, double balanceAfter)
{
    FILE *fp = fopen(TRANSACTIONS_FILE, "a");     /* "a" = append */

    if (fp == NULL)
    {
        return 0;
    }

    fprintf(fp, "%d %s %.2f %.2f\n", accountNumber, type, amount, balanceAfter);

    if (fclose(fp) != 0)
    {
        return 0;
    }
    return 1;
}

/* Keeps the last maxCount transactions of one account, oldest first.
 * A missing file just means no history. */
int getRecentTransactions(int accountNumber, Transaction recent[], int maxCount)
{
    FILE *fp = fopen(TRANSACTIONS_FILE, "r");
    Transaction t;
    int fileAccount;
    int count = 0;
    int i;

    if (fp == NULL)
    {
        return 0;
    }

    /* %15s fits in type[16] */
    while (fscanf(fp, "%d %15s %lf %lf",
                  &fileAccount, t.type, &t.amount, &t.balanceAfter) == 4)
    {
        if (fileAccount != accountNumber)
        {
            continue;
        }

        if (count == maxCount)
        {
            /* Full: shift left to drop the oldest */
            for (i = 1; i < maxCount; i++)
            {
                recent[i - 1] = recent[i];
            }
            count--;
        }

        recent[count] = t;
        count++;
    }

    fclose(fp);
    return count;
}

/* ---------- Safe keyboard input ----------
 * scanf("%d") loops forever if the user types letters, so we read a
 * whole line with fgets and check it ourselves. */

/* Prints prompt, reads one line. Returns 0 if the line was too long.
 * If input is closed (Ctrl+D / Ctrl+Z) the program exits; data is already saved. */
static int readLine(const char *prompt, char *buffer, int size)
{
    int length;
    int ch;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, size, stdin) == NULL)
    {
        printf("\nInput ended. Exiting program.\n");
        exit(0);
    }

    length = (int)strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
        return 1;
    }

    if (feof(stdin))
    {
        return 1;
    }

    /* Too long: discard the rest of the line */
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
    return 0;
}

/* Reads a whole number (menu choice, account number, PIN) */
int readInt(const char *prompt, int *value)
{
    char text[INPUT_SIZE];
    char *end;
    long number;

    if (!readLine(prompt, text, INPUT_SIZE))
    {
        return 0;
    }

    errno = 0;
    number = strtol(text, &end, 10);

    if (end == text)
    {
        return 0;
    }

    while (isspace((unsigned char)*end))
    {
        end++;
    }

    if (*end != '\0' || errno == ERANGE || number < INT_MIN || number > INT_MAX)
    {
        return 0;
    }

    *value = (int)number;
    return 1;
}

/* Reads a money amount like 500 or 250.75 (max 2 decimals).
 * A sign is allowed so atm.c can say "cannot be negative". */
int readAmount(const char *prompt, double *value)
{
    char text[INPUT_SIZE];
    const char *p;
    int digits = 0;
    int decimals = 0;
    int seenDot = 0;

    if (!readLine(prompt, text, INPUT_SIZE))
    {
        return 0;
    }

    p = text;
    while (isspace((unsigned char)*p))
    {
        p++;
    }
    if (*p == '+' || *p == '-')
    {
        p++;
    }

    /* Only digits and one '.' are allowed */
    while (*p != '\0')
    {
        if (isdigit((unsigned char)*p))
        {
            digits++;
            if (seenDot)
            {
                decimals++;
            }
        }
        else if (*p == '.' && !seenDot)
        {
            seenDot = 1;
        }
        else
        {
            break;
        }
        p++;
    }

    while (isspace((unsigned char)*p))
    {
        p++;
    }

    if (*p != '\0' || digits == 0 || decimals > 2)
    {
        return 0;
    }

    *value = strtod(text, NULL);
    return 1;
}
