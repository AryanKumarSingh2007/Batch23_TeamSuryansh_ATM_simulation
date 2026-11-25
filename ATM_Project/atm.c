/*
 * atm.c - ATM operations (Member 3)
 * deposit() and withdraw() change the balance in memory only and return 1;
 * main.c then saves the file.
 */

#include <stdio.h>
#include "atm.h"

/* Tolerance for tiny decimal errors (like 0.1 + 0.2) */
#define EPSILON 0.005

/* Reads and validates an amount. 'action' is only used in messages.
 * Returns 1 if valid, 0 if rejected (message already printed). */
static int askForAmount(const char *prompt, const char *action, double *amount)
{
    if (!readAmount(prompt, amount))
    {
        printf("Invalid input. Enter a number such as 500 or 250.50 "
               "(at most 2 decimal places).\n");
        return 0;
    }

    if (*amount < 0)
    {
        printf("Invalid amount: %s amount cannot be negative.\n", action);
        return 0;
    }

    if (*amount == 0)
    {
        printf("Invalid amount: %s amount cannot be zero.\n", action);
        return 0;
    }

    if (*amount > MAX_TRANSACTION_AMOUNT)
    {
        printf("Invalid amount: the maximum per transaction is Rs. %.2f.\n",
               MAX_TRANSACTION_AMOUNT);
        return 0;
    }

    return 1;
}

/* Saves one line of history for the mini statement */
static void recordTransaction(const Account *account, const char *type, double amount)
{
    if (!appendTransaction(account->accountNumber, type, amount, account->balance))
    {
        printf("Warning: the transaction could not be added to the history file.\n");
    }
}

/* Read-only, so the pointer is const */
void checkBalance(const Account *account)
{
    printf("\n===== BALANCE ENQUIRY =====\n");
    printf("Account number : %d\n", account->accountNumber);
    printf("Current balance: Rs. %.2f\n", account->balance);
}

/* Returns 1 if deposited, 0 if rejected */
int deposit(Account *account)
{
    double amount;

    printf("\n===== DEPOSIT =====\n");

    if (!askForAmount("Enter amount to deposit: Rs. ", "Deposit", &amount))
    {
        return 0;
    }

    account->balance = account->balance + amount;
    printf("Rs. %.2f deposited successfully.\n", amount);
    printf("New balance: Rs. %.2f\n", account->balance);

    recordTransaction(account, "DEPOSIT", amount);
    return 1;
}

/* Returns 1 if withdrawn, 0 if rejected */
int withdraw(Account *account)
{
    double amount;

    printf("\n===== WITHDRAWAL =====\n");

    if (!askForAmount("Enter amount to withdraw: Rs. ", "Withdrawal", &amount))
    {
        return 0;
    }

    if (amount > account->balance + EPSILON)
    {
        printf("Insufficient balance. You asked for Rs. %.2f but your balance is Rs. %.2f.\n",
               amount, account->balance);
        return 0;
    }

    account->balance = account->balance - amount;
    if (account->balance < 0)
    {
        account->balance = 0;       /* remove tiny rounding error */
    }

    printf("Rs. %.2f withdrawn successfully.\n", amount);
    printf("New balance: Rs. %.2f\n", account->balance);

    recordTransaction(account, "WITHDRAW", amount);
    return 1;
}

/* Shows the last few transactions of this account */
void miniStatement(const Account *account)
{
    Transaction recent[MAX_HISTORY];
    int count;
    int i;

    count = getRecentTransactions(account->accountNumber, recent, MAX_HISTORY);

    printf("\n===== MINI STATEMENT =====\n");
    printf("Account number: %d\n", account->accountNumber);

    if (count == 0)
    {
        printf("No transactions recorded yet.\n");
    }
    else
    {
        printf("Last %d transaction(s), oldest first:\n\n", count);
        printf("%-4s %-10s %12s %14s\n", "No.", "Type", "Amount", "Balance After");
        for (i = 0; i < count; i++)
        {
            printf("%-4d %-10s %12.2f %14.2f\n",
                   i + 1, recent[i].type, recent[i].amount, recent[i].balanceAfter);
        }
    }

    printf("\nCurrent balance: Rs. %.2f\n", account->balance);
}
