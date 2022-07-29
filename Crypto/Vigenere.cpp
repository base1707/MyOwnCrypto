#include <iostream>
#include <string>
#include <limits>
#include <locale>

namespace Vigenere
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
    
    inline bool VerifyKey(const std::wstring& language, const std::wstring& key)
    {
        if (language.empty() || key.empty())
            return false;
        
        auto& a = (language == L"en") ? ALPHABET_EN : ALPHABET_RU;
        for (auto& it : key)
        {
            if (a.find(it) == std::wstring::npos)
                return false;
        }
        return true;
    }
    
    inline std::wstring Encode(const std::wstring& language, const std::wstring& input, const std::wstring& key)
    {
        if (language.empty() || input.empty() || key.empty())
            return {};
        
        std::wstring result;
        std::size_t pos{};
        std::size_t kpos{};
        
        // Select symbols
        auto& a = (language == L"en") ? ALPHABET_EN : ALPHABET_RU;
        auto& n = (language == L"en") ? ALPHABET_EN_SIZE : ALPHABET_RU_SIZE;
        
        // Iterate source message
        for (auto& it : input)
        {
            pos = a.find(ToLower(it));
            
            if (pos == std::wstring::npos)
            {
                // Unknown symbols
                result += it;
                continue;
            }
            
            // (m + k) mod n
            result += a[(pos + a.find(key[kpos])) % n];
            
            // Key iteration
            ++kpos; if (kpos >= key.size()) kpos = 0;
            
            // If original symbol is upper, encrypted symbol is upper also
            if (IsUpper(it))
            {
                wchar_t& c = result.back();
                c = ToUpper(c);
            }
        }
        return result;
    }
    
    inline std::wstring Decode(const std::wstring& language, const std::wstring& input, const std::wstring& key)
    {
        if (language.empty() || input.empty() || key.empty())
            return {};
        
        std::wstring result;
        std::size_t pos{};
        std::size_t kpos{};
        
        // Select symbols
        auto& a = language == L"en" ? ALPHABET_EN : ALPHABET_RU;
        auto& n = language == L"en" ? ALPHABET_EN_SIZE : ALPHABET_RU_SIZE;
        
        // Iterate source message
        for (auto& it : input)
        {
            pos = a.find(ToLower(it));
            
            if (pos == std::wstring::npos)
            {
                // Unknown symbols
                result += it;
                continue;
            }
            
            // Fix for incorrct position (<= 0)
            if (a.find(key[kpos]) > pos) pos += n;
            
            // (c - k) mod n
            result += a[(pos - a.find(key[kpos])) % n];
            
            // Key iteration
            ++kpos; if (kpos >= key.size()) kpos = 0;
            
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
    
    // Key
    std::wstring key;
    
    // Input message
    std::wstring source;
    
    // Using symbols
    std::wstring language;
    
    auto CleanInput = []()
    {
        // Clear flags and drop incorrect input
        std::wcin.clear();
        std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    };
    
    // ### Step 1. Select language.
    std::wcout << L"[#] Please, select using locale to start (en, ru): " << std::endl;
    std::wcin >> language;
    
    // Verify language
    while (std::wcin.fail() || language != L"en" && language != L"ru")
    {
        // Prepare std::wcin
        CleanInput();
            
        // Try again
        std::wcout << L"[!] Invalid language, try again: ";
        std::wcin >> key;
    }
        
    // ### Step 2. Prepare key.
    std::wcout << L"[#] Please, enter a key: ";
    std::wcin >> key;
        
    // Verify input & key bounds
    while (std::wcin.fail() || !Vigenere::VerifyKey(language, key))
    {
        // Prepare std::wcin
        CleanInput();
            
        // Try again
        std::wcout << L"[!] Invalid key, try again: ";
        std::wcin >> key;
    }
        
    // ### Step 3. Prepare message.
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
    
    // ### Step 4. Result.
    
    // Encode
    std::wstring result{ Vigenere::Encode(language, source, key) };
    std::wcout << "\t[#] Encoded message: " << result << std::endl;
    
    // Decode
    result = Vigenere::Decode(language, result, key);
    std::wcout << "\t[#] Decoded message: " << result << std::endl;
    
    // Verify user input and decrypted output
    if (source == result)
    {
        std::wcout << L"[#] SUCCESS!" << std::endl;
    }
    return 0;
}
