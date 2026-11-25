# ATM Simulation in C

## 1. Project Title
**ATM Simulation: a menu-driven ATM for the terminal, written in C**
First-semester BTech CSE group project (3 members).

> Educational project only. PINs are saved as plain numbers in a text file, so this is **not** real banking security.

## 2. Description
The program works like a simple ATM. You log in with an account number and a 4-digit PIN, then you can check your balance, deposit, withdraw, see a mini statement, change your PIN and log out. Everything is saved in text files, so your data is still there when you run the program again.

The code is split into three modules so each team member can work on their own file.

## 3. Objectives
- Use structures, arrays, pointers, functions and file handling in one working program.
- Learn to split a program into several files with a shared header.
- Learn to check user input so the program doesn't crash.
- Practise dividing work in a team and joining it at the end.

## 4. Features
1. Login with account number and PIN
2. Maximum 3 wrong login attempts
3. Balance enquiry
4. Deposit (rejects negative, zero and invalid amounts)
5. Withdrawal (also rejects amounts above the balance)
6. PIN change (needs current PIN, new PIN and confirmation)
7. Logout
8. Data saved in `accounts.txt`
9. Mini statement (last 5 transactions)

## 5. Technologies and C Concepts
GCC, standard C libraries only, terminal-based.

| Concept | Where |
|---------|-------|
| `struct`, `typedef` | `Account`, `Transaction` in `atm.h` |
| Arrays | `accounts[MAX_ACCOUNTS]` in `main.c` |
| Pointers | `Account *account` in `deposit`, `withdraw`, `changePin` |
| `const` pointers | functions that only read data |
| Functions and return values | all modules |
| Header file and `#define` | `atm.h` |
| Multiple source files, `static` functions | all files |
| File handling | `fopen`, `fscanf`, `fprintf`, `fclose` in `storage.c` |
| Input checking | `fgets`, `strtol` in `storage.c` |
| `switch`, loops | menus in `main.c` |

## 6. Directory Structure
```
ATM_Project/
├── main.c              menus and program flow
├── atm.c               balance, deposit, withdraw, mini statement
├── auth.c              login and PIN change
├── storage.c           file handling and safe input
├── atm.h               shared struct, constants, prototypes
├── accounts.txt        account data
├── build_and_run.bat   build + run (Windows)
├── build_and_run.sh    build + run (Linux/macOS)
├── README.md
├── TESTING.md
└── transactions.txt    (created automatically on the first transaction)
```

## 7. What Each File Does

**`atm.h`**: the shared "agreement": constants, the `Account` and `Transaction` structures, and the prototypes of all functions. Every `.c` file includes it.

**`storage.c`**: all file work and input helpers.
- `loadAccounts()`: reads `accounts.txt` into the array (returns the count, or -1 on error)
- `saveAccounts()`: writes the array back to the file
- `findAccount()`: finds an account number in the array
- `appendTransaction()` / `getRecentTransactions()`: save and read the history
- `readInt()` / `readAmount()`: safe input. We read a whole line with `fgets` instead of `scanf`, because `scanf("%d")` loops forever if the user types letters.

**`auth.c`**:
- `login()`: asks for account number and PIN, up to 3 times, and returns the account's index (or -1)
- `changePin()`: asks for the current PIN, the new PIN and a confirmation

**`atm.c`**:
- `checkBalance()`, `deposit()`, `withdraw()`, `miniStatement()`
- `deposit()` and `withdraw()` check the amount and change the balance in memory, then return 1 so `main.c` knows to save.

**`main.c`**: loads accounts at the start, shows the menus, calls the other modules, saves after every change and handles logout and exit.

## 8. How the Modules Work Together
```
              main.c  (menus, flow, saving)
             /    |    \
        auth.c  atm.c  storage.c
             \    |    /
        all include atm.h
```
Example, a deposit:
1. `main.c` shows the menu and the user picks 2.
2. `main.c` calls `deposit(current)`, passing a pointer to the logged-in account.
3. `deposit()` reads the amount with `readAmount()` (storage.c), checks it, adds it to the balance and records it.
4. It returns 1, so `main.c` calls `saveAccounts()` to update the file.

## 9. The `Account` Structure
```c
typedef struct
{
    int    accountNumber;   /* e.g. 1001 */
    int    pin;             /* 1000 to 9999 */
    double balance;         /* in rupees */
} Account;
```
`main()` keeps an array of these: `Account accounts[MAX_ACCOUNTS];` (max 10). Each line of `accounts.txt` becomes one array element.

## 10. File Handling and Storage
Variables vanish when a program ends, so we keep data in files.

**`accounts.txt`**, one line per account: `1001 1234 5000.00`
- Start of the program: `fopen("r")` + `fscanf("%d %d %lf")` + `fclose`
- After every change: `fopen("w")` + `fprintf("%d %d %.2f\n")` + `fclose`
- If `fopen` returns `NULL` (missing file), the program shows an error and stops.

**`transactions.txt`**, one line per transaction: `1001 DEPOSIT 1500.00 6500.00`
- New lines are added with `fopen("a")` (append mode). The file is created automatically.
- The mini statement reads it and shows the last 5 lines for the logged-in account.

## 11. Compile (GCC)
In the project folder:
```
gcc main.c atm.c auth.c storage.c -o atm
```
Or just use `build_and_run.bat` (Windows) / `bash build_and_run.sh` (Linux/macOS). They compile and start the program.

## 12. Run
Run it from inside the project folder, because it looks for `accounts.txt` there.

| System | Command |
|--------|---------|
| Linux / macOS | `./atm` |
| Windows | `atm` |

Choose 1 to log in, use the ATM menu, choose 6 to log out, then 2 to exit.

## 13. Default Accounts (for testing)
| Account | PIN | Balance (Rs.) |
|---------|-----|---------------|
| 1001 | 1234 | 5000.00 |
| 1002 | 4321 | 2500.50 |
| 1003 | 1111 | 10000.00 |
| 1004 | 2222 | 750.75 |
| 1005 | 9999 | 0.00 |

Keep a backup copy of `accounts.txt` so you can restore these after testing.

## 14. Limitations and Future Improvements
**Limitations**
- Not secure: PINs are stored and typed in plain view.
- PIN must be 1000 to 9999 (so `0123` isn't allowed).
- After 3 wrong logins the program exits, but restarting gives 3 new tries. Nothing is locked permanently.
- Maximum 10 accounts, and no way to add accounts from the menu.
- Balance uses `double`, so tiny rounding errors are possible (output is rounded to 2 decimals).
- Maximum Rs. 100000 per transaction.

**Ideas for later**
- Savings and Current accounts
- Transfers between accounts
- Daily withdrawal limit
- Hidden PIN input and encrypted PINs
- Permanent account lock after failed attempts
- Date and time in the history

## 15. Team Division
| Member | Files | Work |
|--------|-------|------|
| **Member 1: Storage** | `storage.c`, `atm.h` | `Account` struct and shared prototypes, load/save/find functions, history files, safe input, `accounts.txt` |
| **Member 2: Authentication** | `auth.c` | `login()` with 3 attempts, `changePin()` |
| **Member 3: ATM operations** | `atm.c` | balance, deposit, withdraw, mini statement, amount checks |
| **Everyone** | `main.c`, docs | menus, joining the modules, running the tests |

**Working in parallel**
1. Member 1 finishes `atm.h` first (about 10 minutes) and shares it.
2. Each person writes their own file using the function prototypes in `atm.h`. Test it with a small temporary `main` of your own.
3. Put all files in one folder, compile together and run the tests in `TESTING.md`.
