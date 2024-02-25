int count_digits(int number)
{
    int digits = 1;

    while ((number /= 10) > 0)
        digits++;

    return digits;
}

void int_to_string(unsigned value, char *str)
{
    char *cur_conv = str + 9;

    do
    {
        // Convert current digit to character and store it.
        *cur_conv = 48 + (value % 10);
        cur_conv--;
    } while ((value /= 10) > 0);

    cur_conv++;

    // Left-align string.
    char *cur_align = str;
    while (cur_conv <= str + 9)
    {
        *cur_align = *cur_conv;
        cur_align++;
        cur_conv++;
    }

    *cur_align = 0;
}

char nibble_to_hex(unsigned char nibble)
{
    if (nibble >= 0 && nibble <= 9)
        return 48 + nibble; // 48 = '0'
    else
        return 65 + (nibble - 10); // 41 = 'A'
}

void int_to_hex_string(unsigned value, char *str)
{
    str[0] = '0';
    str[1] = 'x';

    for (int i = 0; i < 8; i++)
    {
        unsigned char nibble = value >> (28 - (i * 4)) & 0xF;
        str[2 + i] = nibble_to_hex(nibble);
    }

    str[10] = 0;
}