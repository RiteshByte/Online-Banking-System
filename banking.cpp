#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Bank {
private:
    string name, accNo;
    float balance;
    bool debitCard, creditCard;
    float loanAmount;
    string pin; // Security PIN

    // Encrypt PIN (simple Caesar Cipher)
    string encrypt(string p) {
        for (char &c : p) c += 3;
        return p;
    }

    // Decrypt PIN
    string decrypt(string p) {
        for (char &c : p) c -= 3;
        return p;
    }

    // Update permanent bank database file
    void updateMasterFile() {
        // First, read all existing data (to remove duplicate entries for this account)
        ifstream fin("BankDatabase.txt");
        string line, allData = "";
        bool updated = false;

        while (getline(fin, line)) {
            if (line.find("Account No: " + accNo) == string::npos) {
                allData += line + "\n";
            }
        }
        fin.close();

        // Add or update this account’s record
        allData += "Name: " + name +
                   " | Account No: " + accNo +
                   " | Balance: ₹" + to_string(balance) +
                   " | Loan: ₹" + to_string(loanAmount) +
                   " | Debit: " + (debitCard ? "Yes" : "No") +
                   " | Credit: " + (creditCard ? "Yes" : "No") + "\n";

        // Save updated data back to master file
        ofstream fout("BankDatabase.txt");
        fout << allData;
        fout.close();
    }

public:
    Bank() {
        balance = 0;
        debitCard = false;
        creditCard = false;
        loanAmount = 0;
    }

    bool accountExists(string ac) {
        ifstream fin(ac + ".txt");
        return fin.good();
    }

    void createAccount() {
        cout << "\nEnter Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter Account Number: ";
        cin >> accNo;

        if (accountExists(accNo)) {
            cout << "\n⚠ Account number exists! Try another.\n";
            return;
        }

        cout << "Set 4-digit PIN: ";
        cin >> pin;

        balance = 0;
        debitCard = false;
        creditCard = false;
        loanAmount = 0;

        saveToFile();
        updateMasterFile(); // Update central database
        cout << "\n✅ Account Created Successfully & Secured with PIN!\n";
    }

    void saveToFile() {
        ofstream fout(accNo + ".txt");
        fout << name << endl;
        fout << accNo << endl;
        fout << balance << endl;
        fout << debitCard << endl;
        fout << creditCard << endl;
        fout << loanAmount << endl;
        fout << encrypt(pin) << endl;
        fout.close();
    }

    bool loadAccount(bool askPin = true) {
        cout << "\nEnter Account Number: ";
        cin >> accNo;

        ifstream fin(accNo + ".txt");
        if (!fin) {
            cout << "\n❌ Account not found!\n";
            return false;
        }

        fin >> ws;
        getline(fin, name);
        fin >> accNo >> balance >> debitCard >> creditCard >> loanAmount;
        string encryptedPin;
        fin >> encryptedPin;
        fin.close();

        pin = decrypt(encryptedPin);

        if (askPin) {
            string enteredPin;
            cout << "Enter PIN: ";
            cin >> enteredPin;
            if (enteredPin != pin) {
                cout << "\n❌ Incorrect PIN! Access Denied.\n";
                return false;
            }
        }
        return true;
    }

    void checkBalance() {
        if (loadAccount()) {
            cout << "\n👤 Account Holder: " << name;
            cout << "\n💰 Balance: ₹" << balance << "\n";
        }
    }

    void addCash() {
        if (loadAccount()) {
            float amt;
            cout << "\nEnter amount to deposit: ";
            cin >> amt;
            balance += amt;
            saveToFile();
            updateMasterFile();
            cout << "\n✅ Amount Added Successfully!\n";
        }
    }

    void withdraw() {
        if (loadAccount()) {
            float amt;
            cout << "\nEnter amount to withdraw: ";
            cin >> amt;
            if (amt > balance) {
                cout << "\n❌ Insufficient Balance!\n";
            } else {
                balance -= amt;
                saveToFile();
                updateMasterFile();
                cout << "\n✅ Withdrawal Successful!\n";
            }
        }
    }

    void applyDebitCard() {
        if (loadAccount()) {
            if (!debitCard) {
                debitCard = true;
                saveToFile();
                updateMasterFile();
                cout << "\n✅ Debit Card Applied Successfully!\n";
            } else {
                cout << "\n⚠ You already have a debit card!\n";
            }
        }
    }

    void applyCreditCard() {
        if (loadAccount()) {
            if (!creditCard) {
                creditCard = true;
                saveToFile();
                updateMasterFile();
                cout << "\n✅ Credit Card Applied Successfully!\n";
            } else {
                cout << "\n⚠ You already have a credit card!\n";
            }
        }
    }

    void takeLoan() {
        if (loadAccount()) {
            float amt;
            cout << "\nEnter loan amount: ";
            cin >> amt;
            loanAmount += amt;
            balance += amt;
            saveToFile();
            updateMasterFile();
            cout << "\n✅ Loan Approved! Amount added to balance.\n";
        }
    }

    void payLoan() {
        if (loadAccount()) {
            if (loanAmount <= 0) {
                cout << "\nℹ No outstanding loan to pay!\n";
                return;
            }

            float amt;
            cout << "\nEnter amount to pay for loan: ";
            cin >> amt;

            if (amt > balance) {
                cout << "\n❌ Insufficient Balance to pay loan!\n";
            } else if (amt > loanAmount) {
                cout << "\n⚠ Amount exceeds loan amount! Paying only required amount.\n";
                balance -= loanAmount;
                loanAmount = 0;
                saveToFile();
                updateMasterFile();
                cout << "\n✅ Loan fully paid!\n";
            } else {
                balance -= amt;
                loanAmount -= amt;
                saveToFile();
                updateMasterFile();
                cout << "\n✅ Loan Payment Successful!\n";
            }
        }
    }

    void showDetails() {
        if (loadAccount()) {
            cout << "\n===== ACCOUNT DETAILS =====\n";
            cout << "👤 Name: " << name << "\n";
            cout << "🏦 Account No: " << accNo << "\n";
            cout << "💰 Balance: ₹" << balance << "\n";
            cout << "💳 Debit Card: " << (debitCard ? "Yes" : "No") << "\n";
            cout << "💳 Credit Card: " << (creditCard ? "Yes" : "No") << "\n";
            cout << "📄 Loan Taken: ₹" << loanAmount << "\n";
        }
    }
};

int main() {
    Bank b;
    int choice;

    do {
        cout << "\n========== 🔒 ONLINE BANKING SYSTEM ==========\n";
        cout << "1. Create New Account\n";
        cout << "2. Check Balance\n";
        cout << "3. Add Cash\n";
        cout << "4. Withdraw Money\n";
        cout << "5. Apply Debit Card\n";
        cout << "6. Apply Credit Card\n";
        cout << "7. Take Loan\n";
        cout << "8. Pay Loan\n";     
        cout << "9. Account Details\n";
        cout << "10. Exit\n";
        cout << "Enter option: ";
        cin >> choice;

        switch (choice) {
        case 1: b.createAccount(); break;
        case 2: b.checkBalance(); break;
        case 3: b.addCash(); break;
        case 4: b.withdraw(); break;
        case 5: b.applyDebitCard(); break;
        case 6: b.applyCreditCard(); break;
        case 7: b.takeLoan(); break;
        case 8: b.payLoan(); break;
        case 9: b.showDetails(); break;
        case 10: cout << "\n🙏 Thank You for Using Secure Online Banking!\n"; break;
        default: cout << "\n❌ Invalid option!\n";
        }
    } while (choice != 10);

    return 0;
}
