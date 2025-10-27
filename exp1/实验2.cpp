#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cctype>
using namespace std;

/* 顺序栈 */
template <typename T>
class Stack
{
public:
    explicit Stack(int sz = 200)
    {
        elem = new T[sz];
        top = -1;
        capacity = sz;
    }
    ~Stack() { delete[] elem; }
    bool empty() const { return top == -1; }
    bool full() const { return top == capacity - 1; }
    void push(const T &x)
    {
        if (full())
            throw "栈溢出";
        elem[++top] = x;
    }
    T pop()
    {
        if (empty())
            throw "栈下溢";
        return elem[top--];
    }
    T &peek()
    {
        if (empty())
            throw "栈空";
        return elem[top];
    }

private:
    T *elem;
    int top;
    int capacity;
};

#define N_OPTR 9
typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    FAC,
    L_P,
    R_P,
    EOE
} Operator;

// 运算符优先级表（栈顶运算符 \ 当前运算符）
const char pri[N_OPTR][N_OPTR] = {
    /* +  -  *  /  ^  !  (  )  \0 */
    {'>', '>', '<', '<', '<', '<', '<', '>', '>'}, // +
    {'>', '>', '<', '<', '<', '<', '<', '>', '>'}, // -
    {'>', '>', '>', '>', '<', '<', '<', '>', '>'}, // *
    {'>', '>', '>', '>', '<', '<', '<', '>', '>'}, // /
    {'>', '>', '>', '>', '>', '<', '<', '>', '>'}, // ^（幂运算，右结合）
    {'>', '>', '>', '>', '>', '>', ' ', '>', '>'}, // !（阶乘，单目）
    {'<', '<', '<', '<', '<', '<', '<', '=', ' '}, // (
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // )
    {'<', '<', '<', '<', '<', '<', '<', ' ', '='}  // EOE（结束符）
};

// 字符转运算符索引
int op2idx(char c)
{
    switch (c)
    {
    case '+':
        return ADD;
    case '-':
        return SUB;
    case '*':
        return MUL;
    case '/':
        return DIV;
    case '^':
        return POW;
    case '!':
        return FAC;
    case '(':
        return L_P;
    case ')':
        return R_P;
    case '\0':
    case '#':
        return EOE;
    default:
        return -1;
    }
}

// 优先级关系转数字（<:1, =:2, >:3）
int relOp(char r)
{
    if (r == '<')
        return 1;
    if (r == '=')
        return 2;
    if (r == '>')
        return 3;
    return 0;
}

// 计算阶乘（辅助函数）
double factorial(int n)
{
    if (n < 0)
        throw "阶乘负数错误";
    double res = 1;
    for (int i = 1; i <= n; ++i)
        res *= i;
    return res;
}

// 运算核心（支持双目+单目运算符）
double calc(double a, char op, double b = 0)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if (fabs(b) < 1e-12)
            throw "除零错误";
        return a / b;
    case '^':
        return pow(a, b); // 幂运算
    case '!':
        return factorial((int)a); // 阶乘（仅支持整数）
    default:
        throw "非法运算符";
    }
}

// 字符串转double
double str2double(const string &s)
{
    return strtod(s.c_str(), NULL);
}

/* 预处理：1.去空白 2.全角转半角 3.处理负号（插入0） */
string preprocess(const string &in)
{
    string out;
    for (size_t i = 0; i < in.size(); ++i)
    {
        unsigned char c = in[i];
        // 1. 跳过所有空白（全角空格、半角空格、TAB等）
        if (isspace(c))
            continue;
        if (c == 0xe3 && i + 2 < in.size() && in[i + 1] == 0x80 && in[i + 2] == 0x80)
        {
            i += 2;
            continue;
        }
        // 2. 全角字符转半角（数字、运算符、括号）
        if (c >= 0xff01 && c <= 0xff5e)
        {                      // 全角符号范围
            out += c - 0xfee0; // 全角→半角（差值0xfee0）
            continue;
        }
        // 3. 全角括号单独处理（避免上述范围遗漏）
        if (c == 0xe3 && i + 2 < in.size())
        {
            if (in[i + 1] == 0x80 && in[i + 2] == 0x88)
            {
                out += '(';
                i += 2;
                continue;
            }
            if (in[i + 1] == 0x80 && in[i + 2] == 0x89)
            {
                out += ')';
                i += 2;
                continue;
            }
        }
        out += c;
    }
    // 4. 处理负号：在开头或'('后添加0（如 "-1"→"0-1"，"( -2"→"(0-2"）
    string res;
    for (size_t i = 0; i < out.size(); ++i)
    {
        if (out[i] == '-' && (i == 0 || out[i - 1] == '(' || op2idx(out[i - 1]) != -1))
        {
            res += '0'; // 插入0，将负号转为双目运算符
        }
        res += out[i];
    }
    return res;
}

/* 函数调用处理（sin/cos/tan/log/ln/sqrt） */
double callFunc(const string &name, double arg)
{
    if (name == "sin")
        return sin(arg);
    if (name == "cos")
        return cos(arg);
    if (name == "tan")
        return tan(arg);
    if (name == "log")
    { // 常用对数（底10）
        if (arg <= 0)
            throw "log参数必须为正";
        return log10(arg);
    }
    if (name == "ln")
    { // 自然对数（底e）
        if (arg <= 0)
            throw "ln参数必须为正";
        return log(arg);
    }
    if (name == "sqrt")
    {
        if (arg < 0)
            throw "sqrt参数不能为负";
        return sqrt(arg);
    }
    throw "未知函数";
}

/* 表达式求值核心 */
double evaluate(const string &expr)
{
    string src = preprocess(expr);
    Stack<double> opnd;   // 操作数栈
    Stack<char> optr;     // 运算符栈
    optr.push('#');       // 栈底哨兵
    string s = src + '#'; // 表达式末尾添加哨兵
    size_t i = 0;

    while (i < s.size())
    {
        char c = s[i];
        // 1. 解析数字（含小数点）
        if (isdigit(c) || c == '.')
        {
            string numStr;
            while (i < s.size() && (isdigit(s[i]) || s[i] == '.'))
            {
                numStr += s[i++];
            }
            opnd.push(str2double(numStr));
            continue;
        }
        // 2. 解析函数（字母开头，如sin、log）
        if (isalpha(c))
        {
            string fname;
            while (i < s.size() && isalpha(s[i]))
            {
                fname += s[i++];
            }
            if (s[i] != '(')
                throw "函数后必须跟'('";
            ++i;         // 跳过'('
            int dep = 1; // 括号深度（处理嵌套括号）
            string subExpr;
            while (i < s.size() && dep > 0)
            {
                if (s[i] == '(')
                    dep++;
                else if (s[i] == ')')
                    dep--;
                if (dep > 0)
                    subExpr += s[i++];
            }
            if (dep != 0)
                throw "函数括号不匹配";
            ++i;                            // 跳过')'
            double arg = evaluate(subExpr); // 递归计算函数参数
            opnd.push(callFunc(fname, arg));
            continue;
        }
        // 3. 处理运算符（含哨兵#）
        char topOp = optr.peek();
        int idx1 = op2idx(topOp), idx2 = op2idx(c);
        if (idx1 < 0 || idx2 < 0)
            throw "非法字符";

        int rel = relOp(pri[idx1][idx2]);
        if (rel == 1)
        { // 栈顶优先级低：当前运算符入栈
            optr.push(c);
            i++;
        }
        else if (rel == 2)
        { // 优先级相等（如()、##）：弹出栈顶，跳过当前字符
            optr.pop();
            i++;
        }
        else if (rel == 3)
        { // 栈顶优先级高：执行运算
            char op = optr.pop();
            // 区分单目运算符（!）和双目运算符
            if (op == '!')
            {
                double a = opnd.pop();
                opnd.push(calc(a, op)); // 单目运算，仅传a和op
            }
            else
            {
                double b = opnd.pop();
                double a = opnd.pop();
                opnd.push(calc(a, op, b)); // 双目运算，传a、op、b
            }
        }
        else
        {
            throw "表达式语法错误";
        }
    }
    // 最终结果校验（操作数栈应只剩一个结果，运算符栈为空）
    double ans = opnd.pop();
    if (!opnd.empty() || !optr.empty())
        throw "表达式异常";
    return ans;
}

/* 测试函数 */
int main()
{
    // 测试用例（覆盖基础运算、函数、负号、阶乘、全角）
    const char *tests[] = {
        "1 + 2 * 3",         // 基础运算
        "(1 + 2) * 3",       // 括号
        "3.5 / 2 + 1",       // 小数
        "sin(0) + cos(0)",   // 三角函数
        "ln(2.718281828)",   // 自然对数
        "sqrt(16) * 2 + 1",  // 平方根
        "1 + 2*(3+4)/5 - 6", // 混合运算
        "tan(3.14159265/4)", // 正切（π/4≈0.785）
        "log(100)",          // 常用对数
        "-1 + 2",            // 负号（开头）
        "1 + (-2)",          // 负号（括号内）
        "5!",                // 阶乘
        "2^3",               // 幂运算
        "１＋２×３",         // 全角运算符
        "1 + + 2",           // 非法表达式（连续+）
        "5 / 0"              // 除零错误
    };

    int n = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < n; ++i)
    {
        cout << "表达式：" << tests[i] << endl;
        try
        {
            cout << "结果 = " << evaluate(tests[i]) << endl;
        }
        catch (const char *e)
        {
            cout << "错误: " << e << " => 式子无效" << endl;
        }
        cout << "-------------------------" << endl;
    }

    /* 交互模式（空行退出） */
    cout << "请输入表达式（空行退出）：" << endl;
    string line;
    while (getline(cin, line))
    {
        if (line.empty())
            break;
        try
        {
            cout << "结果 = " << evaluate(line) << endl;
        }
        catch (const char *e)
        {
            cout << "错误: " << e << endl;
        }
        cout << "-------------------------" << endl;
    }

    return 0;
}