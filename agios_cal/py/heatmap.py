import numpy as np
import pandas as pd
import seaborn as sns
import sys
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
    exit()
df = pd.read_excel(sys.argv[1])
print(sum(df['Ecart']))

df2 = df.groupby('Zone').apply(lambda x: np.sort(x.Nom)[:-4:-1])

df2.to_excel("ech.xlsx")
print(df2.to_latex())

# pv = df.pivot_table(index='Zone', columns='Catégorie', values='Valeur en PPH', aggfunc='sum')
# pv['Total par zone'] = pv.sum(axis=1)
# pv.loc['Total par catégorie']= pv.sum()
# f, ax = plt.subplots(figsize=(22, 9))
# sns.heatmap(pv,fmt='.2f', annot=True)#, cmap="Blues_r")#, cmap=sns.cubehelix_palette(as_cmap=True, reverse=True))
# plt.savefig('fig.png', dpi=500)



# df_neg = df[df['Ecart'] < 0]
# pv_neg = df_neg.pivot_table(index='Zone', columns='Catégorie', values='Valeur en PPH', aggfunc='sum')
# pv_neg['Total par zone'] = pv_neg.sum(axis=1)
# pv_neg.loc['Total par catégorie']= pv_neg.sum()
# f1, ax1 = plt.subplots(figsize=(18, 8))
# sns.heatmap(pv_neg,fmt='.2f', annot=True, cmap="Blues_r")#, cmap=sns.cubehelix_palette(as_cmap=True, reverse=True))
# plt.savefig('fig_neg.png', dpi=500)
# df_pos = df[df['Ecart'] > 0]
# pv_pos = df_pos.pivot_table(index='Zone', columns='Catégorie', values='Valeur en PPH', aggfunc='sum')
# pv_pos['Total par zone'] = pv_pos.sum(axis=1)
# pv_pos.loc['Total par catégorie']= pv_pos.sum()
# f2, ax2 = plt.subplots(figsize=(18, 8))
# sns.heatmap(pv_pos,fmt='.2f', annot=True, cmap="Blues")#, cmap=sns.cubehelix_palette(as_cmap=True, reverse=True))
# plt.savefig('fig_pos.png', dpi=500)

# ax.set_xticklabels(ax.get_xticklabels(), rotation=25, horizontalalignment='right')
# pv_neg['Total par zone'] = pv_neg.sum(axis=1)
# pv_neg.loc['Total par catégorie']= pv_neg.sum()

# ax1.set_xticklabels(ax1.get_xticklabels(), rotation=25, horizontalalignment='right')
# plt.savefig('fig_pos.png', dpi=500)


# df_box = df[df['Catégorie']=="Médicament (29.747%)"]
# f3, ax3 = plt.subplots(figsize=(15, 8))
# sns.boxenplot(data=df_box, x="Valeur en PPH", y="Catégorie", scale="linear")
# f4, ax4 = plt.subplots(figsize=(16, 8))
# sns.boxenplot(data=df, x="Catégorie", y="Valeur en PPH", scale="linear")


# sns.boxplot(data=df, x="Catégorie", y="Valeur en PPH")

# import plotly.express as px

# # df = px.data.tips()

# fig = px.box(df_box, x="Catégorie", y="Valeur en PPH")  
# fig.update_traces(quartilemethod="exclusive") # or "inclusive", or "linear" by default
# fig.show()





# pv_neg['Total par zone'] = pv_neg.sum(axis=1)
# pv_neg.loc['Total par catégorie']= pv_neg.sum()

# f1, ax1 = plt.subplots(figsize=(15, 6))
# sns.heatmap(pv_neg,fmt='.2f', annot=True)#, cmap=sns.cubehelix_palette(as_cmap=True, reverse=True))
# ax1.set_xticklabels(ax1.get_xticklabels(), rotation=0,
#                         horizontalalignment='right')


# pv_pos = df_pos.pivot_table(index='Zone', columns='Catégorie', values='Valeur en PPH', aggfunc='sum').fillna(0)
# # pv_pos['Total par zone'] = pv_pos.sum(axis=1)
# # pv_pos.loc['Total par catégorie'] = pv_pos.sum()

# f2, ax2 = plt.subplots(figsize=(15, 6))
# sns.heatmap(pv_pos, fmt='.2f', annot=True)#, cmap=sns.cubehelix_palette(as_cmap=True))

# cond = [
#     (df['Ecart'] < 0),
#     (df['Ecart'] > 0),
#     (df['Ecart'] == 0)
# ]
# val = ['Négative', 'Postive', 'Non']
# df['Tag'] = np.select(cond, val)
# pv = df.pivot_table(index='Zone', columns='Catégorie', values='Valeur en PPH', aggfunc='sum').fillna(0)
# f3, ax3 = plt.subplots(figsize=(15, 6))
# sns.heatmap(pv, fmt='.2f', annot=True)#, cmap=sns.cubehelix_palette(as_cmap=True, reverse=True))

# drop_ind = df[df['Ecart'] == 0].index
# df_d = df.drop(drop_ind)
# print(df_d.head(15))

plt.show()