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

// Function to convert UTF-8 string to UTF-32 string using ICU
std::optional<std::u32string> ConvertUTF8ToUTF32(const std::string& input) {
    try {
        // Convert UTF-8 string to UnicodeString
        icu::UnicodeString unicode_string = icu::UnicodeString::fromUTF8(input);
        
        // Create UTF-32 string by extracting each Unicode code point
        std::u32string utf32_string;
        for (int32_t i = 0; i < unicode_string.length();) {
            // Extract the code point at the current position
            UChar32 code_point = unicode_string.char32At(i);
            
            // Append the code point to the UTF-32 string
            utf32_string.push_back(static_cast<char32_t>(code_point));
            
            // Move to the next code point (which may be multiple UTF-16 code units)
            i += U16_LENGTH(code_point);
        }
        
        return utf32_string;
    } catch (const std::exception& e) {
        std::cerr << "ICU conversion error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

// Function to compare two UTF-32 strings and report differences
bool CompareUTF32Strings(const std::u32string& str1, const std::u32string& str2) {
    if (str1.length() != str2.length()) {
        std::cerr << "Strings have different lengths: " << str1.length() << " vs " << str2.length() << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < str1.length(); ++i) {
        if (str1[i] != str2[i]) {
            std::cerr << "Mismatch at position " << i << ": " 
                      << "U+" << std::hex << static_cast<uint32_t>(str1[i]) << " vs "
                      << "U+" << static_cast<uint32_t>(str2[i]) << std::dec << std::endl;
            return false;
        }
    }
    
    return true;
}

int main() {
    std::cout << "Starting Conversion Tests" << std::endl;

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

    // Iterate through test strings and test roundtrip conversion
    int passed = 0;
    for (size_t i = 0; i < std::size(test_strings); ++i) {
        std::cout << "\nTest " << i + 1 << ": ";
        
        // Step 1: Convert UTF-32 to UTF-8
        const auto utf8_result = ConvertUTF32ToUTF8(test_strings[i]);
        if (!utf8_result) {
            std::cerr << "UTF-32 to UTF-8 conversion failed!" << std::endl;
            continue;
        }
        
        std::cout << "UTF-32 to UTF-8: " << utf8_result.value() << std::endl;
        
        // Step 2: Convert UTF-8 back to UTF-32
        const auto utf32_result = ConvertUTF8ToUTF32(utf8_result.value());
        if (!utf32_result) {
            std::cerr << "UTF-8 to UTF-32 conversion failed!" << std::endl;
            continue;
        }
        
        // Step 3: Compare original and roundtrip result
        std::cout << "Roundtrip comparison: ";
        if (CompareUTF32Strings(test_strings[i], utf32_result.value())) {
            std::cout << "PASSED - Strings are identical!" << std::endl;
            passed++;
        } else {
            std::cout << "FAILED - Strings differ!" << std::endl;
            
            // Print the original string
            std::cout << "Original (UTF-32): ";
            for (char32_t ch : test_strings[i]) {
                std::cout << "U+" << std::hex << static_cast<uint32_t>(ch) << " ";
            }
            std::cout << std::dec << std::endl;
            
            // Print the roundtrip string
            std::cout << "Roundtrip (UTF-32): ";
            for (char32_t ch : utf32_result.value()) {
                std::cout << "U+" << std::hex << static_cast<uint32_t>(ch) << " ";
            }
            std::cout << std::dec << std::endl;
        }
    }

    std::cout << "\nConversion Tests Complete" << std::endl;
    std::cout << "Results: " << passed << " out of " << std::size(test_strings) << " tests passed." << std::endl;
    
    return 0;
}