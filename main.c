#include "parser.h"
#include "scanner.h"

int main()
{
    int scanner_file_return = set_file(stdin);
    if (scanner_file_return == -1)
    {
        fprintf(stderr, "Failed to open file");
        return 99;
    }

    int parse_return = parse();
    return parse_return;
}
