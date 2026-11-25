/*
 * atm.h - shared header (Member 1)
 * Constants, structures and function prototypes used by all files.
 * Educational project: PINs are NOT stored securely.
 */

#ifndef ATM_H
#define ATM_H

/* ---------- Constants ---------- */
#define MAX_ACCOUNTS            10
#define MAX_LOGIN_ATTEMPTS      3
#define MAX_HISTORY             5           /* rows in mini statement */
#define MIN_PIN                 1000
#define MAX_PIN                 9999
#define MAX_TRANSACTION_AMOUNT  100000.00
#define TYPE_LENGTH             16
#define ACCOUNTS_FILE           "accounts.txt"
#define TRANSACTIONS_FILE       "transactions.txt"

/* ---------- Data types ---------- */

/* One account. File line: accountNumber pin balance */
typedef struct
{
    int    accountNumber;
    int    pin;             /* 4 digits, 1000-9999 */
    double balance;
} Account;

/* One line of history, used by the mini statement */
typedef struct
{
    char   type[TYPE_LENGTH];   /* "DEPOSIT" or "WITHDRAW" */
    double amount;
    double balanceAfter;
} Transaction;

/* ---------- storage.c (Member 1) ---------- */

/* Returns number of accounts loaded, or -1 on error */
int loadAccounts(Account accounts[], int maxAccounts);

/* Returns 1 on success, 0 on error */
int saveAccounts(const Account accounts[], int count);

/* Returns array index, or -1 if not found */
int findAccount(const Account accounts[], int count, int accountNumber);

/* Adds a line to transactions.txt. Returns 1 on success, 0 on error */
int appendTransaction(int accountNumber, const char *type,
                      double amount, double balanceAfter);

/* Gets the last maxCount transactions of an account (oldest first).
 * Returns how many were found. */
int getRecentTransactions(int accountNumber, Transaction recent[], int maxCount);

/* Safe input helpers. Return 1 if valid input, 0 if not. */
int readInt(const char *prompt, int *value);
int readAmount(const char *prompt, double *value);

/* ---------- auth.c (Member 2) ---------- */

/* Returns index of logged-in account, or -1 after 3 failed attempts */
int login(const Account accounts[], int count);

/* Returns 1 if PIN changed, 0 if not */
int changePin(Account *account);

/* ---------- atm.c (Member 3) ---------- */

void checkBalance(const Account *account);
int  deposit(Account *account);      /* 1 if balance changed */
int  withdraw(Account *account);     /* 1 if balance changed */
void miniStatement(const Account *account);

#endif /* ATM_H */
