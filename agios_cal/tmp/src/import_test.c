#include <sqlite3.h>
#include <xlsxio_read.h>
#include <stdio.h>
#include "../libft/libft.h"

// RLV_BP_2023_01
#define BMCE_C "CREATE TABLE IF NOT EXIST RLV_%s_%s_%s(DATE_OP DATE, DATE_VA DATE, DEBIT BIGINT, CREDIT BIGINT, SOLDE BIGINT);"
#define BMCE_H {"Date","Date de valeur","Libellé","Débit","Crédit","Solde"}


static void init_import(char *file) {
    int					rt;
	const	char		*sheetname;
	xlsxioreader		table;
	xlsxioreadersheet	sheet;

	sheetname = NULL;
	if (file) {//&& extention_checker(file) == XLSX) {
		table = xlsxioread_open(file);
		if (table) {
			sheet = xlsxioread_sheet_open(table, sheetname, XLSXIOREAD_SKIP_NONE);
			if (sheet) {
				rt = 0;
				while (++rt < 6)
					xlsxioread_sheet_next_row(sheet);
				while (xlsxioread_sheet_next_row(sheet)) {
                    char *value;
                    rt = 0;
	                value = xlsxioread_sheet_next_cell(sheet);
                    while (value)
	                {
                        if (rt == 7)
                            printf("%s\n", value);
		                rt++;
                        xlsxioread_free(value);
	                    value = xlsxioread_sheet_next_cell(sheet);
                    }
				}
				xlsxioread_sheet_close(sheet);
			}
			xlsxioread_close(table);
		}
	}
}

int main() {
    init_import("/home/anas/clones/agios_cal/ARAB.xlsx");
}