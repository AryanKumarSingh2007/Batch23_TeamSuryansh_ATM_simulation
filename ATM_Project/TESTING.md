# TESTING.md

Test cases for the ATM project. The **Expected** column was taken from a real run of the program. The **Actual** and **Status** columns are empty on purpose: run each test yourselves and fill them in. Nothing here says your team has already run them.

## Before you start
1. Compile: `gcc main.c atm.c auth.c storage.c -o atm` (or use `build_and_run.bat`)
2. Make a backup once: `cp accounts.txt accounts_backup.txt` (Windows: `copy accounts.txt accounts_backup.txt`)
3. **Reset before Tests 5, 6, 10, 12 and 19** because they change data:
   `cp accounts_backup.txt accounts.txt` and `rm -f transactions.txt`
   (Windows: `copy /Y accounts_backup.txt accounts.txt` and `del transactions.txt`)

Accounts: 1001/1234 (Rs.5000.00), 1002/4321 (2500.50), 1003/1111 (10000.00), 1004/2222 (750.75), 1005/9999 (0.00)
Welcome menu: 1 Login, 2 Exit. ATM menu: 1 Balance, 2 Deposit, 3 Withdraw, 4 Mini Statement, 5 Change PIN, 6 Logout.

## Main tests

| No. | Objective | Input | Expected output | Actual | Status |
|-----|-----------|-------|-----------------|--------|--------|
| 1 | Successful login | `1`, account `1001`, PIN `1234` | `Login successful. Welcome, account 1001!` then the ATM menu | | Not Tested |
| 2 | Incorrect PIN | `1`, `1001`, PIN `0000`, then `1001`, `1234` | `Invalid account number or PIN. 2 attempt(s) remaining.` then login succeeds on attempt 2 | | Not Tested |
| 3 | Three failed attempts | `1`, then `1001` with PINs `1`, `2`, `3` | `2 attempt(s) remaining`, `1 attempt(s) remaining`, then `Too many incorrect attempts. Access denied.` and the program exits | | Not Tested |
| 4 | Balance enquiry | Login `1001`/`1234`, menu `1` | `Account number : 1001` and `Current balance: Rs. 5000.00` | | Not Tested |
| 5 | Successful deposit (reset first) | Login `1001`/`1234`, menu `2`, amount `1500` | `Rs. 1500.00 deposited successfully.` and `New balance: Rs. 6500.00`. `accounts.txt` shows `1001 1234 6500.00` | | Not Tested |
| 6 | Successful withdrawal (reset first) | Login `1001`/`1234`, menu `3`, amount `2000` | `Rs. 2000.00 withdrawn successfully.` and `New balance: Rs. 3000.00`. `accounts.txt` shows `1001 1234 3000.00` | | Not Tested |
| 7 | Withdraw more than balance | Login `1004`/`2222`, menu `3`, amount `1000` | `Insufficient balance. You asked for Rs. 1000.00 but your balance is Rs. 750.75.` Balance stays 750.75 | | Not Tested |
| 8 | Negative deposit | Login `1001`/`1234`, menu `2`, amount `-500` | `Invalid amount: Deposit amount cannot be negative.` Balance unchanged | | Not Tested |
| 9 | Negative withdrawal | Login `1001`/`1234`, menu `3`, amount `-200` | `Invalid amount: Withdrawal amount cannot be negative.` Balance unchanged | | Not Tested |
| 10 | Successful PIN change (reset first) | Login `1002`/`4321`, menu `5`, current `4321`, new `5555`, confirm `5555`, logout, then try login with `4321`, then with `5555` | `PIN changed successfully.` Old PIN gives `Invalid account number or PIN.` New PIN gives `Login successful.` File shows `1002 5555 2500.50` | | Not Tested |
| 11 | Logout | Login `1001`/`1234`, menu `6` | `You have been logged out. Thank you for using the ATM!` and the welcome menu appears again | | Not Tested |
| 12 | Data saved after restart (reset first) | Run 1: login `1003`/`1111`, deposit `2500`, withdraw `500`, logout, exit. Run 2: start again, login `1003`/`1111`, menu `1`, then `4` | Run 1 ends with `New balance: Rs. 12000.00`. Run 2 shows `Current balance: Rs. 12000.00` and a mini statement with the DEPOSIT 2500.00 and WITHDRAW 500.00 rows | | Not Tested |

## Extra tests (edge cases)

| No. | Objective | Input | Expected output | Actual | Status |
|-----|-----------|-------|-----------------|--------|--------|
| 13 | Wrong account number | `1`, account `9999`, PIN `1234` | `Invalid account number or PIN. 2 attempt(s) remaining.` | | Not Tested |
| 14 | Zero amounts | Login `1001`/`1234`, deposit `0`, then withdraw `0` | `Deposit amount cannot be zero.` and `Withdrawal amount cannot be zero.` | | Not Tested |
| 15 | Invalid menu choices | Welcome menu: `7`, `abc`. ATM menu: `9`, `x` | `Invalid choice. Please enter 1 or 2.` and `Invalid choice. Please enter a number from 1 to 6.` No crash | | Not Tested |
| 16 | PIN change errors | Login `1001`/`1234`, menu `5`: current `0000`. Again: current `1234`, new `123`. Again: current `1234`, new `5555`, confirm `6666` | `Incorrect PIN. Your PIN was not changed.`, `Invalid PIN. The new PIN must be a 4-digit number (1000 to 9999).`, `The PINs do not match. Your PIN was not changed.` | | Not Tested |
| 17 | Missing `accounts.txt` | Rename the file, then start the program (rename it back afterwards) | `Error: cannot open 'accounts.txt'.` and `The ATM cannot start without a valid account file.` Program exits | | Not Tested |
| 18 | Bad amount format | Deposit `abc`, deposit `10.555`, withdraw `1e3` | `Invalid input. Enter a number such as 500 or 250.50 (at most 2 decimal places).` each time | | Not Tested |
| 19 | Mini statement keeps last 5 (reset first) | Login `1005`/`9999`. Deposit 100, deposit 200, withdraw 50, deposit 300, withdraw 25, deposit 75, then menu `4` | Only 5 rows shown, starting from the 200.00 deposit (the first 100.00 deposit is dropped). `Current balance: Rs. 600.00` | | Not Tested |

## Sample runs
What the user types comes after each prompt. Repeated menus are shortened to `[menu]`.

**Login and balance**
```
Enter your choice: 1

===== LOGIN =====

Attempt 1 of 3
Enter account number: 1001
Enter PIN: 1234

Login successful. Welcome, account 1001!
[ATM menu]
Enter your choice: 1

===== BALANCE ENQUIRY =====
Account number : 1001
Current balance: Rs. 5000.00
```

**Three wrong logins**
```
Attempt 1 of 3
Enter account number: 1001
Enter PIN: 1
Invalid account number or PIN. 2 attempt(s) remaining.

Attempt 2 of 3
Enter account number: 1001
Enter PIN: 2
Invalid account number or PIN. 1 attempt(s) remaining.

Attempt 3 of 3
Enter account number: 1001
Enter PIN: 3
Invalid account number or PIN.

Too many incorrect attempts. Access denied.
Your session has been locked. Please restart the program to try again.
```

**Deposit, withdrawal, then a rejected withdrawal** (account 1003)
```
[ATM menu]
Enter your choice: 2

===== DEPOSIT =====
Enter amount to deposit: Rs. 2500
Rs. 2500.00 deposited successfully.
New balance: Rs. 12500.00

[ATM menu]
Enter your choice: 3

===== WITHDRAWAL =====
Enter amount to withdraw: Rs. 500
Rs. 500.00 withdrawn successfully.
New balance: Rs. 12000.00
```
```
===== WITHDRAWAL =====
Enter amount to withdraw: Rs. 1000
Insufficient balance. You asked for Rs. 1000.00 but your balance is Rs. 750.75.
```
(second box: account 1004)

**Mini statement after restarting** (account 1003)
```
===== MINI STATEMENT =====
Account number: 1003
Last 2 transaction(s), oldest first:

No.  Type             Amount  Balance After
1    DEPOSIT         2500.00       12500.00
2    WITHDRAW         500.00       12000.00

Current balance: Rs. 12000.00
```
