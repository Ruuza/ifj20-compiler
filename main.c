#include "parser.h"
#include "scanner.h"

int main(int argc, char const *argv[])
{
    FILE *srcFile;
    char const *text = argv[1];
    srcFile = fopen(argv[1], "r");

    int scanner_file_return = set_file(srcFile);
    if (scanner_file_return == -1)
    {
        printf("Failed to open file");
        return 1;
    }

    int parse_return = parse();

    return (parse_return);
}
