#include <iostream>
#include <string>
#include <memory>
#include <cctype>
#include <limits>
#include <cstdio>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// ANSI color codes for terminal output
namespace Color {
    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string blue = "\033[34m";
    const std::string magenta = "\033[35m";
    const std::string cyan = "\033[36m";
}

void displayBanner() {
    std::cout << Color::magenta;
    std::cout << R"(
    ╔═══════════════════════════════════════════╗
    ║       ENHANCED CAESAR CIPHER SYSTEM       ║
    ║         -- Decrypt if you dare --         ║
    ╚═══════════════════════════════════════════╝
    )" << Color::reset << std::endl;
}

void displayInstructions(const std::string& mode) {
    std::cout << Color::cyan << "\n=== " << mode << " Instructions ===\n";
    if (mode == "Encryption") {
        std::cout << "1. Enter your message to encrypt\n";
        std::cout << "2. Enter a shift value (1-13) - This will be hidden and is your secret key\n";
        std::cout << "3. Save the encrypted message to share with others!\n";
    } else {  // Decryption
        std::cout << "1. Enter the encrypted message\n";
        std::cout << "2. Enter your guess for the shift value (1-13)\n";
        std::cout << "3. Keep trying different values until you decrypt the message!\n";
    }
    std::cout << Color::reset << std::endl;
}

#ifdef _WIN32
char getHiddenChar() {
    return _getch();
}
#else
char getHiddenChar() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

int getHiddenShiftValue() {
    std::string input;
    char ch;
    while ((ch = getHiddenChar()) != '\n' && ch != '\r') {
        if (ch >= '1' && ch <= '9') {  
            input += ch;
            std::cout << "*";
        } else if ((ch == 127 || ch == 8) && !input.empty()) {  
            input.pop_back();
            std::cout << "\b \b";
        }
    }
    std::cout << std::endl;
    
    try {
        int value = std::stoi(input);
        if (value >= 1 && value <= 13) {
            return value;
        }
    } catch (...) {}
    
    throw std::invalid_argument("Invalid shift value");
}

class CaesarCipher {
private:
    int shiftValue;

    char shiftChar(char ch, bool encrypt) const {
        if (!std::isalpha(ch)) {
            return ch;  
        }
        
        char base = std::isupper(ch) ? 'A' : 'a';
        int shift = encrypt ? shiftValue : -shiftValue;
        return base + ((ch - base + shift + 26) % 26);
    }

public:
    CaesarCipher() : shiftValue(0) {}
    
    void setShiftValue(int value) {
        if (value < 1 || value > 13) {
            throw std::invalid_argument("Invalid shift value");
        }
        shiftValue = value;
    }

    std::string encrypt(const std::string& message) {
        std::string result;
        for (char ch : message) {
            result += shiftChar(ch, true);
        }
        return result;
    }

    std::string decrypt(const std::string& message) {
        std::string result;
        for (char ch : message) {
            result += shiftChar(ch, false);
        }
        return result;
    }
};

int main() {
    displayBanner();
    std::string choice;
    std::string message;
    bool continueProgram = true;
    
    std::cout << Color::cyan << "\nWelcome! This program lets you encrypt messages and challenge others to decrypt them." 
              << Color::reset << std::endl;
    
    while (continueProgram) {
        std::cout << Color::yellow << "\nChoose operation (encrypt/decrypt/quit): " << Color::reset;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == "quit") {
            std::cout << Color::magenta << "\nThank you for using the Enhanced Caesar Cipher System!\n" << Color::reset;
            break;
        } else if (choice == "encrypt") {
            displayInstructions("Encryption");
            
            std::cout << Color::yellow << "Enter message to encrypt: " << Color::reset;
            std::getline(std::cin, message);
            
            try {
                std::cout << Color::yellow << "Enter your secret shift value (1-13, input will be hidden): " << Color::reset;
                int shiftValue = getHiddenShiftValue();
                
                CaesarCipher cipher;
                cipher.setShiftValue(shiftValue);
                std::string encryptedMessage = cipher.encrypt(message);
                std::cout << Color::green << "\nEncrypted Message: " << Color::reset 
                          << encryptedMessage << std::endl;
                
                std::cout << Color::cyan << "\nTip: Save this encrypted message to share with others!" << Color::reset << std::endl;
            } catch (const std::exception& e) {
                std::cerr << Color::red << "Error: " << e.what() << Color::reset << std::endl;
            }
        } else if (choice == "decrypt") {
            displayInstructions("Decryption");
            bool found = false;
            
            std::cout << Color::yellow << "Enter the encrypted message to decrypt: " << Color::reset;
            std::getline(std::cin, message);
            
            while (!found) {
                try {
                    std::cout << Color::yellow << "Enter shift value to try (1-13): " << Color::reset;
                    int attempt;
                    std::cin >> attempt;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    if (attempt < 1 || attempt > 13) {
                        std::cout << Color::red << "Please enter a number between 1 and 13." << Color::reset << std::endl;
                        continue;
                    }
                    
                    CaesarCipher cipher;
                    cipher.setShiftValue(attempt);
                    std::string decryptedMessage = cipher.decrypt(message);
                    std::cout << Color::blue << "Decrypted message: " << Color::reset << decryptedMessage << std::endl;
                    
                    std::cout << "Is this correct? (y/n): ";
                    char response;
                    std::cin >> response;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    if (response == 'y' || response == 'Y') {
                        found = true;
                        std::cout << Color::green << "\nCongratulations! Message successfully decrypted with shift value " 
                                  << attempt << "!" << Color::reset << std::endl;
                    } else {
                        std::cout << Color::blue << "\nTry another shift value? (y/n): " << Color::reset;
                        std::cin >> response;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        if (response != 'y' && response != 'Y') {
                            break;
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << Color::red << "Error: " << e.what() << Color::reset << std::endl;
                    break;
                }
            }
            
            if (!found) {
                std::cout << Color::red << "\nDecryption attempt abandoned." << Color::reset << std::endl;
            }
        } else {
            std::cout << Color::red << "Invalid input. Please enter 'encrypt', 'decrypt', or 'quit'." << Color::reset << std::endl;
        }
    }
    
    return 0;
}
