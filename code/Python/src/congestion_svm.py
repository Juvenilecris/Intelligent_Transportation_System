"""
在合成的交通数据集上，训练SVM分类器预测道路是否处于拥堵状态
"""
import numpy as np
from sklearn import svm
from sklearn.model_selection import train_test_split

# 读取数据
data = np.loadtxt('src/traffic_data.txt')#加载团队自己生成的交通数据集
"""
数据集按照拥堵和非拥堵两种类型的交通数据划定各个数据可能的范围，随机生成600组交通数据，包括总车流量、平均速度、上一辆车的行驶速度、最近一个小时的总车流量、最近一个小时的平均速度
"""

# 将数据分为特征和标签
X = data[:, :15]
y = data[:, 15]

# 划分数据集为训练集和测试集
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2, random_state = 42)

# 创建SVM模型
clf = svm.SVC(kernel='rbf')  # 也可选择其他的核函数，例如 linear, poly, sigmoid

# 训练模型
clf.fit(X_train, y_train)

# 给出测试的交通数据，预测道路是否处于拥堵状态
X_new = np.array([[100, 102, 89, 45, 14,101,101,100, 42, 10, 101, 120, 103, 24, 15]])  # 15个交通数据特征值
predictions = clf.predict(X_new)

print("Predicted labels:", int(predictions.item()))#预测的结果是不处于拥堵状态

# import joblib

# -------------------- 保存模型----------------
# joblib.dump(clf, 'congestion_model.pkl')
#
# ---------------------加载模型----------------
# loaded_model = joblib.load('congestion_model.pkl')
