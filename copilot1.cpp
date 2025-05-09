#include <string>
#include <iostream>
#include <optional>
#include <unicode/unistr.h> // ICU header for UnicodeString

// Function to convert UTF-32 string to UTF-8 string using ICU
std::optional<std::string> ConvertUTF32ToUTF8(const std::u32string& input) {
    try {
        // Use ICU's UnicodeString directly with UTF-32 code points
        // This eliminates the need for manual UTF-16 conversion
        icu::UnicodeString unicode_string;
        for (const char32_t ch : input) {
            // Append each code point directly to the UnicodeString
            // UnicodeString.append() handles proper UTF-16 encoding internally
            unicode_string.append(static_cast<UChar32>(ch));
        }

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
    std::cout << "Starting Conversion" << std::endl;

    // Test strings in UTF-32
    const std::u32string test_strings[] = {
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
        const auto result = ConvertUTF32ToUTF8(test_strings[i]);
        if (result) {
            std::cout << "Conversion successful: " << result.value() << std::endl;
        } else {
            std::cerr << "Conversion failed!" << std::endl;
        }
    }

    std::cout << "Conversion Complete" << std::endl;
    return 0;
}