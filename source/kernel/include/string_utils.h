int count_digits(int number);

/// @brief Converts an unsigned into into a string.
/// @param value The unsigned int to convert.
/// @param str An array of characters of size 11. 10 for the digits and one for the null terminating character.
void int_to_string(unsigned value, char *str);


/// @brief Converts an unsigned into into it's hex representation in a string.
/// @param value The unsigned int to convert.
/// @param str An array of characters of 67. 64 characters for every 4 bits. 2 for "0x" and 1 for the null terminating character.
void int_to_hex_string(unsigned value, char* str);

/// @brief Converts a pointer into into it's hex representation in a string.
/// @param value The unsigned int to convert.
/// @param str An array of characters of 67. 64 characters for every 4 bits. 2 for "0x" and 1 for the null terminating character.
void ptr_to_hex_string(void* value, char* str);