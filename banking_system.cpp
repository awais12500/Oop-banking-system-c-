#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <fstream>
#include <cctype>

using namespace std;

// Forward declarations 
class Transaction;
class Account;
class SavingsAccount;
class CurrentAccount;
class Loan;
class Bank;

// Maximum number of accounts, transactions, and loans
const int MAX_ACCOUNTS = 100;
const int MAX_TRANSACTIONS = 100;
const int MAX_LOANS = 100;

// Helper function to get current date and time as string
string getCurrentDateTime() {
    time_t now = time(0);
    char buffer[80];
    struct tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now);
#else
    localtime_r(&now, &timeinfo);
#endif
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return string(buffer);
}

// Helper function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    while (cin.get() != '\n');
}

// Helper function to format double to 2 decimal places
string formatDouble(double value) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return string(buffer);
}

// Helper function to create a formatted line
string formatLine(int width, char fillChar = '-') {
    string line;
    for (int i = 0; i < width; i++) {
        line += fillChar;
    }
    return line;
}

// Helper function to format a string to fixed width
string formatString(const string& str, int width, bool leftAlign = true) {
    string result = str;
    if (result.length() >= width) {
        return result.substr(0, width);
    }
    if (leftAlign) {
        result.append(width - result.length(), ' ');
    }
    else {
        result = string(width - result.length(), ' ') + result;
    }
    return result;
}

// Helper function to validate PIN format
bool isValidPin(const string& pin) {
    if (pin.length() != 4) return false;
    for (char c : pin) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// Customer class
class Customer {
private:
    string name;
    string address;
    string phone;
    int customerID;
    string pin; // 4-digit PIN code
    static int nextCustomerID;

public:
    Customer() : customerID(0), pin("0000") {}
    Customer(string name, string address, string phone, string pin = "0000")
        : name(name), address(address), phone(phone), pin(pin) {
        customerID = nextCustomerID++;
    }

    void inputDetails() {
        clearInputBuffer();
        cout << "Enter name: ";
        getline(cin, name);
        cout << "Enter address: ";
        getline(cin, address);
        cout << "Enter phone number: ";
        getline(cin, phone);
        customerID = nextCustomerID++;
    }

    void setPin(const string& newPin) {
        pin = newPin;
    }

    void displayDetails() const {
        cout << "Customer ID: " << customerID << endl;
        cout << "Name: " << name << endl;
        cout << "Address: " << address << endl;
        cout << "Phone: " << phone << endl;
    }

    string getName() const { return name; }
    int getCustomerID() const { return customerID; }
    string getPin() const { return pin; }

    static int getNextCustomerID() { return nextCustomerID; }
    static void setNextCustomerID(int id) { nextCustomerID = id; }

    void saveToFile(ofstream& outFile) const {
        outFile << customerID << endl;
        outFile << name << endl;
        outFile << address << endl;
        outFile << phone << endl;
        outFile << pin << endl;
    }

    void loadFromFile(ifstream& inFile) {
        inFile >> customerID;
        inFile.ignore();
        getline(inFile, name);
        getline(inFile, address);
        getline(inFile, phone);
        if (!getline(inFile, pin) || pin.empty()) {
            pin = "0000";
        }
    }
};

int Customer::nextCustomerID = 1000;

// Transaction class
class Transaction {
private:
    int transactionID;
    string dateTime;
    string type;
    double amount;
    int fromAccount;
    int toAccount;
    static int nextTransactionID;

public:
    Transaction() : transactionID(0), amount(0.0), fromAccount(0), toAccount(-1) {}
    Transaction(string type, double amount, int fromAcc, int toAcc = -1)
        : type(type), amount(amount), fromAccount(fromAcc), toAccount(toAcc) {
        transactionID = nextTransactionID++;
        dateTime = getCurrentDateTime();
    }

    void display() const {
        cout << formatString(to_string(transactionID), 5) << " | ";
        cout << formatString(dateTime, 20) << " | ";
        cout << formatString(type, 12) << " | ";
        cout << formatString("$" + formatDouble(amount), 10) << " | ";
        cout << formatString(to_string(fromAccount), 6);
        if (toAccount != -1) {
            cout << " -> " << formatString(to_string(toAccount), 6);
        }
        else {
            cout << "        ";
        }
        cout << endl;
    }

    bool isEmpty() const { return transactionID == 0; }

    static int getNextTransactionID() { return nextTransactionID; }
    static void setNextTransactionID(int id) { nextTransactionID = id; }

    void saveToFile(ofstream& outFile) const {
        outFile << transactionID << endl;
        outFile << dateTime << endl;
        outFile << type << endl;
        outFile << amount << endl;
        outFile << fromAccount << endl;
        outFile << toAccount << endl;
    }

    void loadFromFile(ifstream& inFile) {
        inFile >> transactionID;
        inFile.ignore();
        getline(inFile, dateTime);
        getline(inFile, type);
        inFile >> amount;
        inFile >> fromAccount;
        inFile >> toAccount;
        inFile.ignore();
    }
};

int Transaction::nextTransactionID = 10000;

// Loan class
class Loan {
private:
    int loanID;
    int customerID;
    double principal;
    double interestRate;
    int durationMonths;
    double monthlyPayment;
    double remainingBalance;
    static int nextLoanID;

public:
    Loan() : loanID(0), customerID(0), principal(0.0), interestRate(0.0), durationMonths(0), monthlyPayment(0.0), remainingBalance(0.0) {}
    Loan(int custID, double princ, double rate, int months)
        : customerID(custID), principal(princ), interestRate(rate), durationMonths(months) {
        loanID = nextLoanID++;
        double totalInterest = principal * interestRate * (durationMonths / 12.0);
        remainingBalance = principal + totalInterest;
        monthlyPayment = remainingBalance / durationMonths;
    }

    void makePayment(double amount) {
        if (amount <= 0) {
            cout << "Invalid payment amount!" << endl;
            return;
        }
        if (amount > remainingBalance) {
            cout << "Payment exceeds remaining balance. Paying off $" << formatDouble(remainingBalance) << endl;
            amount = remainingBalance;
        }
        remainingBalance -= amount;
        cout << "Payment of $" << formatDouble(amount) << " applied to loan " << loanID << endl;
        cout << "Remaining balance: $" << formatDouble(remainingBalance) << endl;
    }

    void display() const {
        cout << "\n--- Loan Details ---" << endl;
        cout << "Loan ID: " << loanID << endl;
        cout << "Customer ID: " << customerID << endl;
        cout << "Principal: $" << formatDouble(principal) << endl;
        cout << "Interest Rate: " << interestRate * 100 << "%" << endl;
        cout << "Duration: " << durationMonths << " months" << endl;
        cout << "Monthly Payment: $" << formatDouble(monthlyPayment) << endl;
        cout << "Remaining Balance: $" << formatDouble(remainingBalance) << endl;
    }

    bool isActive() const { return remainingBalance > 0; }
    int getCustomerID() const { return customerID; }
    int getLoanID() const { return loanID; }

    static int getNextLoanID() { return nextLoanID; }
    static void setNextLoanID(int id) { nextLoanID = id; }

    void saveToFile(ofstream& outFile) const {
        outFile << loanID << endl;
        outFile << customerID << endl;
        outFile << principal << endl;
        outFile << interestRate << endl;
        outFile << durationMonths << endl;
        outFile << monthlyPayment << endl;
        outFile << remainingBalance << endl;
    }

    void loadFromFile(ifstream& inFile) {
        inFile >> loanID;
        inFile >> customerID;
        inFile >> principal;
        inFile >> interestRate;
        inFile >> durationMonths;
        inFile >> monthlyPayment;
        inFile >> remainingBalance;
        inFile.ignore();
    }
};

int Loan::nextLoanID = 100000;

// Abstract Account class
class Account {
protected:
    int accountNumber;
    double balance;
    Customer customer;
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount;
    string accountType;
    static int nextAccountNumber;

public:
    Account(Customer cust, string type)
        : customer(cust), balance(0.0), accountType(type), transactionCount(0) {
        accountNumber = nextAccountNumber++;
    }

    virtual ~Account() {}

    virtual void deposit(double amount) = 0;
    virtual bool withdraw(double amount) = 0;
    virtual void display() const = 0;

    void addTransaction(const Transaction& transaction) {
        if (transactionCount < MAX_TRANSACTIONS) {
            transactions[transactionCount] = transaction;
            transactionCount++;
        }
        else {
            cout << "Transaction history full. Cannot record transaction." << endl;
        }
    }

    void displayTransactions() const {
        cout << "\n--- Transaction History for Account " << accountNumber << " ---\n" << endl;
        cout << formatString("ID", 5) << " | ";
        cout << formatString("Date & Time", 20) << " | ";
        cout << formatString("Type", 12) << " | ";
        cout << formatString("Amount", 10) << " | ";
        cout << formatString("Account(s)", 15) << endl;
        cout << formatLine(70) << endl;

        for (int i = 0; i < transactionCount; i++) {
            transactions[i].display();
        }
        if (transactionCount == 0) {
            cout << "No transactions recorded." << endl;
        }
    }

    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    Customer getCustomer() const { return customer; }
    string getAccountType() const { return accountType; }

    static int getNextAccountNumber() { return nextAccountNumber; }
    static void setNextAccountNumber(int number) { nextAccountNumber = number; }

    virtual void saveToFile(ofstream& outFile) const {
        outFile << accountNumber << endl;
        outFile << balance << endl;
        outFile << accountType << endl;
        outFile << transactionCount << endl;
        customer.saveToFile(outFile);
        for (int i = 0; i < transactionCount; i++) {
            transactions[i].saveToFile(outFile);
        }
    }

    virtual void loadFromFile(ifstream& inFile) {
        inFile >> accountNumber;
        inFile >> balance;
        inFile.ignore();
        getline(inFile, accountType);
        inFile >> transactionCount;
        inFile.ignore();
        customer.loadFromFile(inFile);
        for (int i = 0; i < transactionCount; i++) {
            transactions[i].loadFromFile(inFile);
        }
    }
};

int Account::nextAccountNumber = 100;

// SavingsAccount class
class SavingsAccount : public Account {
private:
    double interestRate;
    double minimumBalance;

public:
    SavingsAccount(Customer cust, double initialDeposit = 0.0, double rate = 0.025)
        : Account(cust, "Savings"), interestRate(rate), minimumBalance(500.0) {
        if (initialDeposit > 0) {
            deposit(initialDeposit);
        }
    }

    void deposit(double amount) override {
        if (amount <= 0) {
            cout << "Invalid deposit amount!" << endl;
            return;
        }
        balance += amount;
        Transaction transaction("Deposit", amount, accountNumber);
        addTransaction(transaction);
        cout << "Deposit of $" << formatDouble(amount);
        cout << " to account " << accountNumber << " successful." << endl;
        cout << "New balance: $" << formatDouble(balance) << endl;
    }

    bool withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount!" << endl;
            return false;
        }
        if (balance - amount < minimumBalance) {
            cout << "Withdrawal failed! Must maintain minimum balance of $"
                << formatDouble(minimumBalance) << endl;
            return false;
        }
        balance -= amount;
        Transaction transaction("Withdrawal", amount, accountNumber);
        addTransaction(transaction);
        cout << "Withdrawal of $" << formatDouble(amount);
        cout << " from account " << accountNumber << " successful." << endl;
        cout << "New balance: $" << formatDouble(balance) << endl;
        return true;
    }

    void applyInterest() {
        double interest = balance * interestRate;
        balance += interest;
        Transaction transaction("Interest", interest, accountNumber);
        addTransaction(transaction);
        cout << "Interest applied: $" << formatDouble(interest) << endl;
        cout << "New balance: $" << formatDouble(balance) << endl;
    }

    void display() const override {
        cout << "\n--- Savings Account Details ---" << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Account Type: Savings" << endl;
        cout << "Interest Rate: " << interestRate * 100 << "%" << endl;
        cout << "Minimum Balance: $" << formatDouble(minimumBalance) << endl;
        cout << "Current Balance: $" << formatDouble(balance) << endl;
        cout << "\n--- Customer Details ---" << endl;
        customer.displayDetails();
    }

    void saveToFile(ofstream& outFile) const override {
        Account::saveToFile(outFile);
        outFile << interestRate << endl;
        outFile << minimumBalance << endl;
    }

    void loadFromFile(ifstream& inFile) override {
        Account::loadFromFile(inFile);
        inFile >> interestRate;
        inFile >> minimumBalance;
        inFile.ignore();
    }
};

// CurrentAccount class
class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(Customer cust, double initialDeposit = 0.0, double limit = 1000.0)
        : Account(cust, "Current"), overdraftLimit(limit) {
        if (initialDeposit > 0) {
            deposit(initialDeposit);
        }
    }

    void deposit(double amount) override {
        if (amount <= 0) {
            cout << "Invalid deposit amount!" << endl;
            return;
        }
        balance += amount;
        Transaction transaction("Deposit", amount, accountNumber);
        addTransaction(transaction);
        cout << "Deposit of $" << formatDouble(amount);
        cout << " to account " << accountNumber << " successful." << endl;
        cout << "New balance: $" << formatDouble(balance) << endl;
    }

    bool withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount!" << endl;
            return false;
        }
        if (balance - amount < -overdraftLimit) {
            cout << "Withdrawal failed! Exceeds overdraft limit of $"
                << formatDouble(overdraftLimit) << endl;
            return false;
        }
        balance -= amount;
        Transaction transaction("Withdrawal", amount, accountNumber);
        addTransaction(transaction);
        cout << "Withdrawal of $" << formatDouble(amount);
        cout << " from account " << accountNumber << " successful." << endl;
        cout << "New balance: $" << formatDouble(balance) << endl;
        return true;
    }

    void display() const override {
        cout << "\n--- Current Account Details ---" << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Account Type: Current" << endl;
        cout << "Overdraft Limit: $" << formatDouble(overdraftLimit) << endl;
        cout << "Current Balance: $" << formatDouble(balance) << endl;
        cout << "\n--- Customer Details ---" << endl;
        customer.displayDetails();
    }

    void saveToFile(ofstream& outFile) const override {
        Account::saveToFile(outFile);
        outFile << overdraftLimit << endl;
    }

    void loadFromFile(ifstream& inFile) override {
        Account::loadFromFile(inFile);
        inFile >> overdraftLimit;
        inFile.ignore();
    }
};

// Helper functions for input
int getIntInput() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        while (cin.get() != '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    return value;
}

double getDoubleInput() {
    double value;
    while (!(cin >> value)) {
        cin.clear();
        while (cin.get() != '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    return value;
}

string getPinInput() {
    string pin;
    clearInputBuffer();
    cout << "Enter 4-digit PIN: ";
    getline(cin, pin);
    return pin;
}

// Bank class
class Bank {
private:
    Account* accounts[MAX_ACCOUNTS];
    Loan* loans[MAX_LOANS];
    int accountCount;
    int loanCount;
    string bankName;

public:
    Bank(string name = "OOP Banking System") : bankName(name), accountCount(0), loanCount(0) {
        for (int i = 0; i < MAX_ACCOUNTS; i++) {
            accounts[i] = nullptr;
        }
        for (int i = 0; i < MAX_LOANS; i++) {
            loans[i] = nullptr;
        }
        cout << "Welcome to " << bankName << "!" << endl;
    }

    ~Bank() {
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i] != nullptr) {
                delete accounts[i];
                accounts[i] = nullptr;
            }
        }
        for (int i = 0; i < loanCount; i++) {
            if (loans[i] != nullptr) {
                delete loans[i];
                loans[i] = nullptr;
            }
        }
    }

    bool isPinUnique(const string& pin) const {
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i] && accounts[i]->getCustomer().getPin() == pin) {
                return false;
            }
        }
        return true;
    }

    bool verifyCustomerPin(int accountNumber, const string& pin) const {
        Account* account = findAccount(accountNumber);
        if (account && account->getCustomer().getPin() == pin) {
            return true;
        }
        cout << "Invalid PIN!" << endl;
        return false;
    }

    void createAccount() {
        if (accountCount >= MAX_ACCOUNTS) {
            cout << "Maximum number of accounts reached!" << endl;
            return;
        }
        Customer customer;
        customer.inputDetails();
        string pin;
        bool validPin = false;
        while (!validPin) {
            pin = getPinInput();
            if (!isValidPin(pin)) {
                cout << "PIN must be exactly 4 digits!" << endl;
                continue;
            }
            if (!isPinUnique(pin)) {
                cout << "PIN already in use! Please choose a different PIN." << endl;
                continue;
            }
            validPin = true;
        }
        customer.setPin(pin);
        int choice;
        double initialDeposit;
        cout << "\nSelect Account Type:" << endl;
        cout << "1. Savings Account" << endl;
        cout << "2. Current Account" << endl;
        cout << "Enter choice (1-2): ";
        choice = getIntInput();
        cout << "Enter initial deposit amount: $";
        initialDeposit = getDoubleInput();
        Account* newAccount = nullptr;
        if (choice == 1) {
            newAccount = new SavingsAccount(customer, initialDeposit);
            cout << "\nSavings Account created successfully!" << endl;
        }
        else if (choice == 2) {
            newAccount = new CurrentAccount(customer, initialDeposit);
            cout << "\nCurrent Account created successfully!" << endl;
        }
        else {
            cout << "Invalid choice! Account creation failed." << endl;
            return;
        }
        accounts[accountCount] = newAccount;
        accountCount++;
        cout << "Account Number: " << newAccount->getAccountNumber() << endl;
    }

    Account* findAccount(int accountNumber) const {
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i] && accounts[i]->getAccountNumber() == accountNumber) {
                return accounts[i];
            }
        }
        return nullptr;
    }

    void closeAccount(int accountNumber) {
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i] && accounts[i]->getAccountNumber() == accountNumber) {
                int custID = accounts[i]->getCustomer().getCustomerID();
                for (int j = 0; j < loanCount; j++) {
                    if (loans[j] && loans[j]->getCustomerID() == custID && loans[j]->isActive()) {
                        cout << "Cannot close account. Customer has an active loan." << endl;
                        return;
                    }
                }
                cout << "Account " << accountNumber << " closed." << endl;
                delete accounts[i];
                for (int j = i; j < accountCount - 1; j++) {
                    accounts[j] = accounts[j + 1];
                }
                accounts[accountCount - 1] = nullptr;
                accountCount--;
                return;
            }
        }
        cout << "Account " << accountNumber << " not found!" << endl;
    }

    void depositToAccount(int accountNumber, double amount) {
        Account* account = findAccount(accountNumber);
        if (!account) {
            cout << "Account " << accountNumber << " not found!" << endl;
            return;
        }
        string pin = getPinInput();
        if (!verifyCustomerPin(accountNumber, pin)) {
            return;
        }
        account->deposit(amount);
    }

    void withdrawFromAccount(int accountNumber, double amount) {
        Account* account = findAccount(accountNumber);
        if (!account) {
            cout << "Account " << accountNumber << " not found!" << endl;
            return;
        }
        string pin = getPinInput();
        if (!verifyCustomerPin(accountNumber, pin)) {
            return;
        }
        account->withdraw(amount);
    }

    void transferBetweenAccounts(int fromAccNum, int toAccNum, double amount) {
        if (fromAccNum == toAccNum) {
            cout << "Cannot transfer to the same account!" << endl;
            return;
        }
        Account* fromAccount = findAccount(fromAccNum);
        Account* toAccount = findAccount(toAccNum);
        if (!fromAccount) {
            cout << "Source account " << fromAccNum << " not found!" << endl;
            return;
        }
        if (!toAccount) {
            cout << "Destination account " << toAccNum << " not found!" << endl;
            return;
        }
        string pin = getPinInput();
        if (!verifyCustomerPin(fromAccNum, pin)) {
            return;
        }
        bool withdrawSuccess = fromAccount->withdraw(amount);
        if (withdrawSuccess) {
            toAccount->deposit(amount);
            Transaction transaction("Transfer", amount, fromAccNum, toAccNum);
            fromAccount->addTransaction(transaction);
            toAccount->addTransaction(transaction);
            cout << "Transfer of $" << formatDouble(amount);
            cout << " from account " << fromAccNum << " to account " << toAccNum;
            cout << " completed successfully." << endl;
        }
    }

    void displayAccount(int accountNumber) {
        Account* account = findAccount(accountNumber);
        if (account) {
            account->display();
        }
        else {
            cout << "Account " << accountNumber << " not found!" << endl;
        }
    }

    void displayAccountTransactions(int accountNumber) {
        Account* account = findAccount(accountNumber);
        if (account) {
            account->displayTransactions();
        }
        else {
            cout << "Account " << accountNumber << " not found!" << endl;
        }
    }

    void displayAllAccounts() const {
        if (accountCount == 0) {
            cout << "No accounts found in the system!" << endl;
            return;
        }
        cout << "\n--- All Accounts ---" << endl;
        cout << formatString("Acc No.", 10) << " | ";
        cout << formatString("Type", 10) << " | ";
        cout << formatString("Customer Name", 20) << " | ";
        cout << formatString("Balance", 12) << endl;
        cout << formatLine(60) << endl;
        for (int i = 0; i < accountCount; i++) {
            cout << formatString(to_string(accounts[i]->getAccountNumber()), 10) << " | ";
            cout << formatString(accounts[i]->getAccountType(), 10) << " | ";
            cout << formatString(accounts[i]->getCustomer().getName(), 20) << " | ";
            cout << formatString("$" + formatDouble(accounts[i]->getBalance()), 12) << endl;
        }
    }

    void applyInterestToAllSavings() {
        bool appliedToAny = false;
        for (int i = 0; i < accountCount; i++) {
            SavingsAccount* savingsAccount = dynamic_cast<SavingsAccount*>(accounts[i]);
            if (savingsAccount) {
                savingsAccount->applyInterest();
                appliedToAny = true;
            }
        }
        if (appliedToAny) {
            cout << "Interest applied to all savings accounts." << endl;
        }
        else {
            cout << "No savings accounts found to apply interest." << endl;
        }
    }

    void manageLoans() {
        int accountNumber;
        cout << "Enter account number: ";
        accountNumber = getIntInput();
        Account* account = findAccount(accountNumber);
        if (!account) {
            cout << "Account " << accountNumber << " not found!" << endl;
            return;
        }
        string pin = getPinInput();
        if (!verifyCustomerPin(accountNumber, pin)) {
            return;
        }
        int choice;
        cout << "\n--- Loan Management ---" << endl;
        cout << "1. Apply for a Loan" << endl;
        cout << "2. View Loan Details" << endl;
        cout << "3. Make Loan Payment" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter choice (1-4): ";
        choice = getIntInput();
        if (choice == 1) {
            int custID = account->getCustomer().getCustomerID();
            for (int i = 0; i < loanCount; i++) {
                if (loans[i] && loans[i]->getCustomerID() == custID && loans[i]->isActive()) {
                    cout << "Customer already has an active loan!" << endl;
                    return;
                }
            }
            double principal;
            int duration;
            cout << "Enter loan amount ($1000-$50000): $";
            principal = getDoubleInput();
            if (principal < 1000 || principal > 50000) {
                cout << "Loan amount must be between $1000 and $50000!" << endl;
                return;
            }
            if (principal > 5 * account->getBalance()) {
                cout << "Loan rejected! Amount exceeds 5x account balance ($"
                    << formatDouble(account->getBalance()) << ")." << endl;
                return;
            }
            cout << "Enter loan duration (12-60 months): ";
            duration = getIntInput();
            if (duration < 12 || duration > 60) {
                cout << "Duration must be between 12 and 60 months!" << endl;
                return;
            }
            if (loanCount >= MAX_LOANS) {
                cout << "Maximum number of loans reached!" << endl;
                return;
            }
            loans[loanCount] = new Loan(custID, principal, 0.05, duration);
            account->deposit(principal);
            Transaction transaction("Loan Disbursement", principal, accountNumber);
            account->addTransaction(transaction);
            cout << "Loan approved! $" << formatDouble(principal) << " deposited to account "
                << accountNumber << endl;
            loans[loanCount]->display();
            loanCount++;
        }
        else if (choice == 2) {
            int loanID;
            cout << "Enter loan ID: ";
            loanID = getIntInput();
            for (int i = 0; i < loanCount; i++) {
                if (loans[i] && loans[i]->getLoanID() == loanID) {
                    loans[i]->display();
                    return;
                }
            }
            cout << "Loan " << loanID << " not found!" << endl;
        }
        else if (choice == 3) {
            int loanID;
            double amount;
            cout << "Enter loan ID: ";
            loanID = getIntInput();
            Loan* loan = nullptr;
            for (int i = 0; i < loanCount; i++) {
                if (loans[i] && loans[i]->getLoanID() == loanID) {
                    loan = loans[i];
                    break;
                }
            }
            if (!loan) {
                cout << "Loan " << loanID << " not found!" << endl;
                return;
            }
            cout << "Enter payment amount: $";
            amount = getDoubleInput();
            loan->makePayment(amount);
            if (!loan->isActive()) {
                cout << "Loan fully repaid!" << endl;
            }
        }
        else if (choice == 4) {
            return;
        }
        else {
            cout << "Invalid choice!" << endl;
        }
    }

    void saveToFile(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }
        outFile << bankName << endl;
        outFile << accountCount << endl;
        outFile << Account::getNextAccountNumber() << endl;
        outFile << Customer::getNextCustomerID() << endl;
        outFile << Transaction::getNextTransactionID() << endl;
        outFile << loanCount << endl;
        outFile << Loan::getNextLoanID() << endl;
        for (int i = 0; i < accountCount; i++) {
            outFile << accounts[i]->getAccountType() << endl;
            accounts[i]->saveToFile(outFile);
        }
        for (int i = 0; i < loanCount; i++) {
            loans[i]->saveToFile(outFile);
        }
        outFile.close();
        cout << "Data saved successfully to " << filename << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "Error opening file for reading!" << endl;
            return;
        }
        for (int i = 0; i < accountCount; i++) {
            delete accounts[i];
            accounts[i] = nullptr;
        }
        for (int i = 0; i < loanCount; i++) {
            delete loans[i];
            loans[i] = nullptr;
        }
        accountCount = 0;
        loanCount = 0;
        getline(inFile, bankName);
        inFile >> accountCount;
        int nextAccNum, nextCustID, nextTransID, nextLoanID;
        inFile >> nextAccNum;
        inFile >> nextCustID;
        inFile >> nextTransID;
        inFile >> loanCount;
        inFile >> nextLoanID;
        Account::setNextAccountNumber(nextAccNum);
        Customer::setNextCustomerID(nextCustID);
        Transaction::setNextTransactionID(nextTransID);
        Loan::setNextLoanID(nextLoanID);
        inFile.ignore();
        for (int i = 0; i < accountCount; i++) {
            string accountType;
            getline(inFile, accountType);
            if (accountType == "Savings") {
                accounts[i] = new SavingsAccount(Customer("", "", ""), 0.0);
            }
            else if (accountType == "Current") {
                accounts[i] = new CurrentAccount(Customer("", "", ""), 0.0);
            }
            else {
                cerr << "Unknown account type: " << accountType << endl;
                continue;
            }
            if (accounts[i]) {
                accounts[i]->loadFromFile(inFile);
            }
        }
        for (int i = 0; i < loanCount; i++) {
            loans[i] = new Loan();
            loans[i]->loadFromFile(inFile);
        }
        inFile.close();
        cout << "Data loaded successfully from " << filename << endl;
    }
};

// Main function
int main() {
    Bank bank("OOP Banking System");
    int choice;
    bool running = true;

    while (running) {
        cout << "\n=== OOP Banking System ===" << endl;
        cout << "1. Create New Account" << endl;
        cout << "2. Display Account Details" << endl;
        cout << "3. Deposit Money" << endl;
        cout << "4. Withdraw Money" << endl;
        cout << "5. Transfer Money" << endl;
        cout << "6. Close an Account" << endl;
        cout << "7. Display All Accounts" << endl;
        cout << "8. Display Transaction History" << endl;
        cout << "9. Apply Interest to Savings Accounts" << endl;
        cout << "10. Save Data to File" << endl;
        cout << "11. Load Data from File" << endl;
        cout << "12. Manage Loans" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice (0-12): ";

        choice = getIntInput();

        if (choice == 1) {
            bank.createAccount();
        }
        else if (choice == 2) {
            int accountNumber;
            cout << "Enter account number: ";
            accountNumber = getIntInput();
            bank.displayAccount(accountNumber);
        }
        else if (choice == 3) {
            int accountNumber;
            double amount;
            cout << "Enter account number: ";
            accountNumber = getIntInput();
            cout << "Enter deposit amount: $";
            amount = getDoubleInput();
            bank.depositToAccount(accountNumber, amount);
        }
        else if (choice == 4) {
            int accountNumber;
            double amount;
            cout << "Enter account number: ";
            accountNumber = getIntInput();
            cout << "Enter withdrawal amount: $";
            amount = getDoubleInput();
            bank.withdrawFromAccount(accountNumber, amount);
        }
        else if (choice == 5) {
            int fromAcc, toAcc;
            double amount;
            cout << "Enter source account number: ";
            fromAcc = getIntInput();
            cout << "Enter destination account number: ";
            toAcc = getIntInput();
            cout << "Enter transfer amount: $";
            amount = getDoubleInput();
            bank.transferBetweenAccounts(fromAcc, toAcc, amount);
        }
        else if (choice == 6) {
            int accountNumber;
            cout << "Enter account number to close: ";
            accountNumber = getIntInput();
            bank.closeAccount(accountNumber);
        }
        else if (choice == 7) {
            bank.displayAllAccounts();
        }
        else if (choice == 8) {
            int accountNumber;
            cout << "Enter account number: ";
            accountNumber = getIntInput();
            bank.displayAccountTransactions(accountNumber);
        }
        else if (choice == 9) {
            bank.applyInterestToAllSavings();
        }
        else if (choice == 10) {
            string filename;
            cout << "Enter filename to save data: ";
            clearInputBuffer();
            getline(cin, filename);
            bank.saveToFile(filename);
        }
        else if (choice == 11) {
            string filename;
            cout << "Enter filename to load data: ";
            clearInputBuffer();
            getline(cin, filename);
            bank.loadFromFile(filename);
        }
        else if (choice == 12) {
            bank.manageLoans();
        }
        else if (choice == 0) {
            cout << "Thank you for using OOP Banking System. Goodbye!" << endl;
            running = false;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
} 







