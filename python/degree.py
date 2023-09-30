import matplotlib.pyplot as plt

# 准备数据
x = list(range(3, 1001, 5))
y_insert = [float(line.strip()) for line in open('../build/src/performance_insert')]
y_search = [float(line.strip()) for line in open('../build/src/performance_search')]
y_delete = [float(line.strip()) for line in open('../build/src/performance_delete')]
titles = ['insert', 'search', 'delete']
# 创建一个图形窗口
fig, axs = plt.subplots(3)

# 绘制曲线
axs[0].plot(x, y_insert)
axs[1].plot(x, y_search)
axs[2].plot(x, y_delete)

# 设置每个坐标轴的标签
for i in range(len(axs)):
    axs[i].set_xlabel('degree')
    axs[i].set_ylabel('time')
    axs[i].set_title(titles[i])

# # 设置标题
# fig.suptitle('performance')
# 保存图像
plt.savefig('../doc/res/performance')

# 显示图形
plt.show()