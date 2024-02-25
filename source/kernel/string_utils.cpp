int count_digits(int number)
{
    int digits = 1;

    while((number /= 10) > 0)
        digits++;

    return digits;
}