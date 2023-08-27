"""
const sleep = (delay) => new Promise((resolve) => setTimeout(resolve, delay))

String.prototype.esc_txt = function () {
    return this.replaceAll('\n', '').replaceAll('\t','');
}

jQuery.fn.extend({
    getDataTable: function (){
        let i = 0;
        let thisTable = $(this);
        let data = {}
        data['thead'] = [];
        thisTable.find("thead th").each ( function () {
            data['thead'].push($(this).text().esc_txt());
        });
        data['tbody'] = [];
        thisTable.find("tbody tr").each ( function () {
            let temp = {}
            $(this).find("td").each ( function (j) {
                temp[data['thead'][j]] = $(this).text().esc_txt();
            });
            i++;
            if (i > 5 && i < 26) {
                data['tbody'].push(temp);
            }
        });
        return data['tbody'];
    },
    getTable: function () {
        return $(this).getDataTable()['tbody'];
    }
});

let data = []

for (let i = 1; i < 138; i++) {
    await sleep(500)
    tablePaginate('39345_00b0e56924bae5f617d4ba4d038a50a9', i);
    await sleep(500)
    data.push($("table").getDataTable());
}
"""

import openpyxl
import pandas as pd
import sys

def write_to_excel(data, file_path):
    workbook = openpyxl.Workbook()
    sheet = workbook.active
    for row_index, row_data in enumerate(data, start=1):
        for col_index, cell_value in enumerate(row_data, start=1):
            sheet.cell(row=row_index, column=col_index, value=cell_value)
    workbook.save(file_path)

if len(sys.argv) != 2:
    print("f\n")
    exit()

f = open(sys.argv[1], "r")
data = []
row = []
for line in f:
    if 'Nom' in line:
        arg = line.split("\"")
        row.append(arg[3:4][0])
    elif 'PPH' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(float(arg))
    elif 'Quantité en stock' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(int(arg))
    elif 'Quantité réelle' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(int(arg))
    elif 'Ecart' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(int(arg))
    elif 'Valeur en PPV' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(float(arg))
    elif 'Valeur en PPH' in line:
        arg = line.split("\"")
        arg = arg[3:4][0].replace(' ', '').replace(',', '.')
        row.append(int(arg))
    if len(row) == 7:
        data.append(row)
        row = []

df = pd.DataFrame(data, columns=['Nom', 'PPH', 'Quantité en stock', 'Quantité réelle', 'Ecart', 'Valeur en PPV', 'Valeur en PPH'])
df1 = df.drop_duplicates()
df.to_excel("Inv_dup.xlsx")
print(df['Quantité en stock'].sum())
print(df['Quantité réelle'].sum())
print(df['Ecart'].sum())
print(sum(df['PPH']*df['Ecart']))
print(df['Valeur en PPV'].sum())
print("#####")
print(df1['Quantité en stock'].sum())
print(df1['Quantité réelle'].sum())
print(df1['Ecart'].sum())
print(sum(df1['PPH']*df1['Ecart']))
print(df1['Valeur en PPV'].sum())
#df.to_excel("Inv.xlsx")

