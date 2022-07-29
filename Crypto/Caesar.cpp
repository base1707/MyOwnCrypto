#include <iostream>
#include <string>
#include <limits>
#include <locale>

namespace Caesar
{
    inline const std::wstring ALPHABET_EN = L"abcdefghijklmnopqrstuvwxyz";
    inline const std::wstring ALPHABET_RU = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    
    constexpr std::size_t ALPHABET_EN_SIZE = 26;
    constexpr std::size_t ALPHABET_RU_SIZE = 33;
    
    // char + value for lower, char - value for upper
    constexpr uint32_t UPPER_VALUE = L'A' - L'a';
    
    inline bool IsUpper(wchar_t c)
    {
        return c >= L'A' && c <= L'Z' || c >= L'А' && c <= L'Я';
    }
    
    inline wchar_t ToUpper(wchar_t c)
    {
        return (c >= L'a' && c <= L'z' || c >= L'а' && c <= L'я') ? static_cast<wchar_t>(c - UPPER_VALUE) : c;
    }
    
    inline wchar_t ToLower(wchar_t c)
    {
        return (c >= L'A' && c <= L'Z' || c >= L'А' && c <= L'Я') ? static_cast<wchar_t>(c + UPPER_VALUE) : c;
    }
    
    inline std::wstring Encode(const std::wstring& input, std::size_t key)
    {
        if (input.empty())
            return {};
        
        std::wstring result;
        std::size_t pos{};
        
        // Iterate source message
        for (auto& it : input)
        {
            // English symbols
            pos = ALPHABET_EN.find(ToLower(it));
            
            if (pos != std::wstring::npos)
            {
                // (x + k) mod n
                result += ALPHABET_EN[(pos + key) % ALPHABET_EN_SIZE];
            }
            else
            {
                // Russian symbols
                pos = ALPHABET_RU.find(ToLower(it));
                
                if (pos != std::wstring::npos)
                {
                    // (x + k) mod n
                    result += ALPHABET_RU[(pos + key) % ALPHABET_RU_SIZE];
                }
                else
                {
                    // Unknown symbols
                    result += it;
                    continue;
                }
            }
            
            // If original symbol is upper, encrypted symbol is upper also
            if (IsUpper(it))
            {
                wchar_t& c = result.back();
                c = ToUpper(c);
            }
        }
        return result;
    }
    
    inline std::wstring Decode(const std::wstring& input, std::size_t key)
    {
        if (input.empty())
            return {};
        
        std::wstring result;
        std::size_t pos{};
        
        // Iterate encrypted message
        for (auto& it : input)
        {
            // English symbols
            pos = ALPHABET_EN.find(ToLower(it));
            
            if (pos != std::wstring::npos)
            {
                // Fix for incorrct position (<= 0)
                if (key > pos) pos += ALPHABET_EN_SIZE;
                
                // (y - k) mod n
                result += ALPHABET_EN[(pos - key) % ALPHABET_EN_SIZE];
            }
            else
            {
                // Russian symbols
                pos = ALPHABET_RU.find(ToLower(it));
                
                if (pos != std::wstring::npos)
                {
                    // Fix for incorrct position (<= 0)
                    if (key > pos) pos += ALPHABET_RU_SIZE;
                    
                    // (y - k) mod n
                    result += ALPHABET_RU[(pos - key) % ALPHABET_RU_SIZE];
                }
                else
                {
                    // Unknown symbols
                    result += it;
                    continue;
                }
            }
            
            // If original symbol is upper, encrypted symbol is upper also
            if (IsUpper(it))
            {
                wchar_t& c = result.back();
                c = ToUpper(c);
            }
        }
        return result;
    }
}

int main(int, char* [])
{
    // Use UTF-8 for both languages
    std::ios_base::sync_with_stdio(false);
    std::wcin.imbue(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale("en_US.UTF-8"));
    
    // Key length
    std::size_t key{};
    
    // Input message
    std::wstring source;
    
    auto CleanInput = []()
    {
        // Clear flags and drop incorrect input
        std::wcin.clear();
        std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    };
        
    // ### Step 1. Prepare key.
    std::wcout << L"[#] Please, enter a key size: ";
    std::wcin >> key;
        
    // Verify input & key bounds
    while (std::wcin.fail() || key == 0 || key > Caesar::ALPHABET_RU_SIZE)
    {
        // Prepare std::wcin
        CleanInput();
            
        // Try again
        std::wcout << L"[!] Invalid key size, try again: ";
        std::wcin >> key;
    }
        
    // ### Step 2. Prepare message.
    std::wcout << L"[#] Please, enter a message: ";
        
    // Prepare std::wcin
    CleanInput();
    std::getline(std::wcin, source);
    
    while (std::wcin.fail())
    {
        // Prepare std::wcin
        CleanInput();
            
        // Try again
        std::wcout << L"[!] Invalid symbols in message, try again: ";
        std::getline(std::wcin, source);
    }
    
    // ### Step 3. Result.
    
    // Encode
    std::wstring result{ Caesar::Encode(source, key) };
    std::wcout << "\t[#] Encoded message: " << result << std::endl;
    
    // Decode
    result = Caesar::Decode(result, key);
    std::wcout << "\t[#] Decoded message: " << result << std::endl;
    
    // Verify user input and decrypted output
    if (source == result)
    {
        std::wcout << L"[#] SUCCESS!" << std::endl;
    }
    return 0;
}
