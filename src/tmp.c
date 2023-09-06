#include <stdio.h>
#include <stdlib.h>
#include <libxls/xls.h>

int main() {
    xlsWorkBook* pWorkbook;
    xlsWorkSheet* pWorksheet;
    xlsCell* pCell;
    const char* xlsFilePath = "example.xls";
    pWorkbook = xls_open(xlsFilePath, "UTF-8");
    if (pWorkbook == NULL) {
        fprintf(stderr, "Failed to open Excel file.\n");
        return 1;
    }
    pWorksheet = xls_getWorkSheet(pWorkbook, 0);
    if (pWorksheet == NULL) {
        fprintf(stderr, "Failed to open worksheet.\n");
        xls_close(pWorkbook);
        return 1;
    }

    for (int row = 0; row <= pWorksheet->rows.lastrow; row++) {
        for (int col = 0; col <= pWorksheet->rows.lastcol; col++) {

            pCell = xls_cell(pWorksheet, row, col);

            if (pCell != NULL) {
                printf("Cell[%d][%d]: %s\n", row, col, pCell->str);
            }
        }
    }
    xls_close(pWorkbook);
    return 0;
}
