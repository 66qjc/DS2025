#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;
/* 计算柱状图中最大矩形面积（单调栈） */
int largestRectangleArea(vector<int> &heights)
{
    heights.push_back(0); // 哨兵，保证栈最后能清空
    stack<int> stk;       // 存放下标
    int maxArea = 0;
    for (int i = 0; i < (int)heights.size(); ++i)
    {
        while (!stk.empty() && heights[stk.top()] > heights[i])
        {
            int h = heights[stk.top()];
            stk.pop();
            int left = stk.empty() ? -1 : stk.top();
            int w = i - left - 1;
            maxArea = max(maxArea, h * w);
        }
        stk.push(i);
    }
    heights.pop_back(); // 恢复原始数组
    return maxArea;
}
/* 生成 [l, r] 区间的随机整数 */
int randInt(int l, int r)
{
    return l + rand() % (r - l + 1);
}
int main()
{
    srand((unsigned)time(NULL));
    const int TEST_CASES = 10;
    cout << "随机测试开始" << endl;
    for (int t = 1; t <= TEST_CASES; ++t)
    {
        int n = randInt(1, 105);
        vector<int> heights(n);
        for (int i = 0; i < n; ++i)
            heights[i] = randInt(0, 104);
        cout << "第 " << t << " 组：n = " << n;
        cout << "， 生成的数组中的数：";
        for (size_t i = 0; i < heights.size(); ++i)
            cout << heights[i] << " ";
        cout << "，最大矩形面积 = " << largestRectangleArea(heights) << endl;
    }
    cout << "随机测试结束" << endl;
    cout << "请输入高度数组 heights=[ ]" << endl;
    string line;
    getline(cin, line);
    vector<int> heights;
    int num = 0;
    bool inNum = false;
    bool neg = false;
    for (int i = 0; i < (int)line.size(); ++i)
    {
        char c = line[i];
        if (c == '[')
        {
            inNum = false;
            continue;
        }
        if (c == ']')
            break;
        if (isdigit(c))
        {
            num = num * 10 + (c - '0');
            inNum = true;
        }
        else if (c == ',' || c == ' ')
        {
            if (inNum)
            {
                if (neg)
                    num = -num;
                heights.push_back(num);
                num = 0;
                neg = false;
                inNum = false;
            }
        }
    }
    if (inNum) // 最后一个数字
    {
        if (neg)
            num = -num;
        heights.push_back(num);
    }
    if (heights.empty())
    {
        cout << "未读到任何高度，程序退出。" << endl;
        return 0;
    }
    int area = largestRectangleArea(heights);
    cout << "输出：" << area << endl;
    return 0;
}
