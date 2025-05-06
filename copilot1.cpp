#include <string>
#include <iostream>
#include <optional>
#include <unicode/unistr.h> // ICU header for UnicodeString

// Helper function to convert UTF-32 to UTF-16
std::u16string ConvertUTF32ToUTF16(const std::u32string& input) {
    std::u16string utf16_string;
    for (char32_t ch : input) {
        if (ch <= 0xFFFF) {
            utf16_string.push_back(static_cast<char16_t>(ch));
        } else if (ch <= 0x10FFFF) {
            char16_t high_surrogate = static_cast<char16_t>(0xD800 + ((ch - 0x10000) >> 10));
            char16_t low_surrogate = static_cast<char16_t>(0xDC00 + ((ch - 0x10000) & 0x3FF));
            utf16_string.push_back(high_surrogate);
            utf16_string.push_back(low_surrogate);
        } else {
            throw std::runtime_error("Invalid UTF-32 character detected.");
        }
    }
    return utf16_string;
}

// Function to convert UTF-32 string to UTF-8 string using ICU
std::optional<std::string> ConvertUTF32ToUTF8(const std::u32string& input) {
    try {
        // Convert UTF-32 to UTF-16
        std::u16string utf16_string = ConvertUTF32ToUTF16(input);

        // Convert UTF-16 to ICU's UnicodeString
        icu::UnicodeString unicode_string(reinterpret_cast<const UChar*>(utf16_string.data()), utf16_string.size());

        // Convert UnicodeString to UTF-8
        std::string utf8_string;
        unicode_string.toUTF8String(utf8_string);

        return utf8_string;
    } catch (const std::exception& e) {
        std::cerr << "ICU conversion error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

int main() {
    std::cout << "Starting Conversion"  << std::endl;

    // Test strings in UTF-32
    
    std::u32string test_strings[] = {
        U"Hello, World!",                  // Basic ASCII
        U"こんにちは世界",                  // Japanese (Hello, World)
        U"Привет, мир!",                  // Russian (Hello, World)
        U"你好，世界！",                    // Chinese (Hello, World)
        U"안녕하세요, 세계!",               // Korean (Hello, World)
        U"😀😃😄😁😆😅😂🤣",                  // Emojis
        U"𐍈𐍉𐍊𐍋𐍌",                     // Gothic script (Supplementary Plane)
        U"مرحبا بالعالم",                 // Arabic (Hello, World)
        U"हैलो वर्ल्ड",                   // Hindi (Hello, World)
        U"שלום עולם"                      // Hebrew (Hello, World)
    };

    // Iterate through test strings and attempt conversion
    for (size_t i = 0; i < std::size(test_strings); ++i) {
        std::cout << "Test " << i + 1 << ": ";
        auto result = ConvertUTF32ToUTF8(test_strings[i]);
        if (result) {
            std::cout << "Conversion successful: " << result.value() << std::endl;
        } else {
            std::cerr << "Conversion failed!" << std::endl;
        }
    }

    std::cout << "Conversion Complete"  << std::endl;
    return 0;
}