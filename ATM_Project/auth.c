/*
 * auth.c - login and PIN change (Member 2)
 */

#include <stdio.h>
#include "atm.h"

/* Up to 3 attempts. Returns the account's index, or -1 if all fail.
 * Same error for wrong account or wrong PIN, so nothing is revealed. */
int login(const Account accounts[], int count)
{
    int attempt;

    printf("\n===== LOGIN =====\n");

    for (attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++)
    {
        int accountNumber = 0;
        int pin = 0;
        int index = -1;
        int inputOk;
        int attemptsLeft = MAX_LOGIN_ATTEMPTS - attempt;

        printf("\nAttempt %d of %d\n", attempt, MAX_LOGIN_ATTEMPTS);

        /* && skips the PIN prompt if the account number is invalid */
        inputOk = readInt("Enter account number: ", &accountNumber) &&
                  readInt("Enter PIN: ", &pin);

        if (inputOk)
        {
            index = findAccount(accounts, count, accountNumber);
        }

        /* Account must exist AND PIN must match */
        if (index != -1 && accounts[index].pin == pin)
        {
            printf("\nLogin successful. Welcome, account %d!\n", accountNumber);
            return index;
        }

        printf("Invalid account number or PIN.");
        if (attemptsLeft > 0)
        {
            printf(" %d attempt(s) remaining.", attemptsLeft);
        }
        printf("\n");
    }

    printf("\nToo many incorrect attempts. Access denied.\n");
    return -1;
}

/* Changes the PIN through a pointer (edits the real account, not a copy).
 * Returns 1 if changed, 0 if cancelled. */
int changePin(Account *account)
{
    int currentPin;
    int newPin;
    int confirmPin;

    printf("\n===== CHANGE PIN =====\n");

    /* 1. Check current PIN */
    if (!readInt("Enter current PIN: ", &currentPin) || currentPin != account->pin)
    {
        printf("Incorrect PIN. Your PIN was not changed.\n");
        return 0;
    }

    /* 2. New PIN must be 4 digits and different */
    if (!readInt("Enter new 4-digit PIN: ", &newPin) ||
        newPin < MIN_PIN || newPin > MAX_PIN)
    {
        printf("Invalid PIN. The new PIN must be a 4-digit number (1000 to 9999).\n");
        return 0;
    }

    if (newPin == account->pin)
    {
        printf("The new PIN must be different from the current PIN.\n");
        return 0;
    }

    /* 3. Confirm */
    if (!readInt("Confirm new PIN: ", &confirmPin) || confirmPin != newPin)
    {
        printf("The PINs do not match. Your PIN was not changed.\n");
        return 0;
    }

    account->pin = newPin;
    printf("PIN changed successfully.\n");
    return 1;
}
