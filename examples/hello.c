struct Pair
{
    int x;
    int y;
};

int addFive(int x)
{
    int result = x + 5;
    return x;
}

int main(int argc, char** argv)
{
    addFive(8);
    return 0;
}
