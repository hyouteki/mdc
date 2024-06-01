#include "mdc.h"

int main() {
    char *filename = "./examples/sample.md";
    Mdc_TokenList *list = Mdc_TokenizeFile(filename);
    Mdc_ConvertToHtml(stdout, list);
    Mdc_TokenList_Free(list);
    return 0;
}
