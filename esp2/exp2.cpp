#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cctype>
#include <map>
#include <cstring>
#include <iomanip>
#include <string>

using namespace std;

// 位图类，用于高效表示二进制序列
class Bitmap
{
private:
    unsigned char *M; // 位图存储空间
    size_t N;         // 位图空间大小（单位：字节）
    size_t _sz;       // 有效位数

    void init(size_t n)
    {
        N = (n + 7) / 8;
        M = new unsigned char[N];
        memset(M, 0, N);
        _sz = 0;
    }

    // 非const方法，用于扩展位图
    void expand(size_t k)
    {
        if (k < 8 * N)
            return;
        size_t oldN = N;
        unsigned char *oldM = M;
        init(2 * k);
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }

public:
    Bitmap(size_t n = 8)
    {
        init(n);
    }

    ~Bitmap()
    {
        delete[] M;
    }

    size_t size() const
    {
        return _sz;
    }

    void set(size_t k)
    {
        expand(k);
        _sz++;
        M[k >> 3] |= (0x80 >> (k & 0x07));
    }

    void clear(size_t k)
    {
        expand(k);
        _sz--;
        M[k >> 3] &= ~(0x80 >> (k & 0x07));
    }

    // 修复1：const版本的test，只检查已存在的位，不扩展
    bool test(size_t k) const
    {
        // 超出当前位图范围直接返回false
        if (k >= 8 * N)
            return false;
        return (M[k >> 3] & (0x80 >> (k & 0x07))) != 0;
    }

    // 修复2：const版本的bits2string，不扩展位图
    string bits2string(size_t n) const
    {
        string s;
        // 只处理0~min(n-1, 8*N-1)的位，超出部分补0
        size_t maxBit = min(n, 8 * N);
        for (size_t i = 0; i < maxBit; i++)
        {
            s += test(i) ? '1' : '0';
        }
        // 不足n位补0
        while (s.size() < n)
        {
            s += '0';
        }
        return s;
    }

    // 新增：非const版本，允许扩展后生成字符串（供需要扩展的场景）
    string bits2string_and_expand(size_t n)
    {
        if (n > 8 * N)
            expand(n - 1);
        return bits2string(n);
    }
};

// 二叉树节点类
class BinNode
{
public:
    char ch;        // 字符
    size_t freq;    // 频率
    BinNode *left;  // 左子节点
    BinNode *right; // 右子节点

    BinNode(char c = '\0', size_t f = 0, BinNode *l = nullptr, BinNode *r = nullptr)
        : ch(c), freq(f), left(l), right(r) {}

    // 修复3：递归释放子节点，避免内存泄漏
    ~BinNode()
    {
        delete left;
        delete right;
    }
};

// 二叉树类
class BinTree
{
private:
    BinNode *root;

public:
    BinTree(BinNode *r = nullptr) : root(r) {}
    ~BinTree() { delete root; }

    bool isEmpty() const { return root == nullptr; }
    BinNode *getRoot() const { return root; }
};

// Huffman树节点类
class HuffNode : public BinNode
{
public:
    HuffNode(char c = '\0', size_t f = 0, BinNode *l = nullptr, BinNode *r = nullptr)
        : BinNode(c, f, l, r) {}
};

// Huffman编码树
class HuffTree
{
private:
    BinNode *root;
    map<char, string> encodingMap;
    vector<size_t> freqMap; // 保存26个字母的频率

    void buildEncodingMap(BinNode *node, string code)
    {
        if (node == nullptr)
            return;
        if (node->ch != '\0')
        {
            encodingMap[node->ch] = code;
            return;
        }
        buildEncodingMap(node->left, code + '0');
        buildEncodingMap(node->right, code + '1');
    }

public:
    HuffTree(const string &text)
    {
        // 统计字符频率（只考虑26个字母，不区分大小写）
        vector<size_t> freq(26, 0);
        for (char c : text)
        {
            if (isalpha(c))
            {
                c = tolower(c);
                freq[c - 'a']++;
            }
        }
        freqMap = freq; // 保存频率

        // 构建优先队列（最小堆）
        priority_queue<pair<size_t, BinNode *>,
                       vector<pair<size_t, BinNode *>>,
                       greater<pair<size_t, BinNode *>>>
            pq;
        for (size_t i = 0; i < 26; i++)
        {
            if (freq[i] > 0)
            {
                pq.push({freq[i], new BinNode('a' + i, freq[i])});
            }
        }

        // 构建Huffman树
        while (pq.size() > 1)
        {
            auto node1 = pq.top();
            pq.pop();
            auto node2 = pq.top();
            pq.pop();
            BinNode *newNode = new BinNode('\0', node1.first + node2.first, node1.second, node2.second);
            pq.push({node1.first + node2.first, newNode});
        }

        root = pq.empty() ? nullptr : pq.top().second;
        buildEncodingMap(root, "");
    }

    ~HuffTree()
    {
        delete root; // 会触发BinNode的析构，递归释放所有子节点
    }

    string getEncoding(char c) const
    {
        if (encodingMap.find(c) != encodingMap.end())
        {
            return encodingMap.at(c); // 使用at()而非[]，避免修改map
        }
        return "";
    }

    string encodeText(const string &text) const
    {
        string encoded;
        for (char c : text)
        {
            if (isalpha(c))
            {
                c = tolower(c);
                string code = getEncoding(c);
                if (!code.empty())
                {
                    encoded += code;
                }
            }
        }
        return encoded;
    }

    // 修复4：const函数中使用map::at()替代operator[]
    double getAverageCodeLength() const
    {
        double totalLength = 0;
        double totalChars = 0;
        for (size_t i = 0; i < 26; i++)
        {
            if (freqMap[i] > 0)
            {
                char c = 'a' + i;
                // 使用at()，仅查询不修改，符合const语义
                string code = encodingMap.at(c);
                totalLength += code.length() * freqMap[i];
                totalChars += freqMap[i];
            }
        }
        // 避免除以0
        return totalChars == 0 ? 0 : totalLength / totalChars;
    }
};

// Huffman编码串类型，基于Bitmap
class HuffCode
{
private:
    Bitmap bitmap;
    size_t length;

public:
    HuffCode(size_t n = 8) : bitmap(n), length(0) {}

    void appendBit(int bit)
    {
        if (bit == 1)
        {
            bitmap.set(length);
        }
        length++;
    }

    string toString() const
    {
        return bitmap.bits2string(length);
    }

    size_t size() const
    {
        return length;
    }
};

// 读取演讲原文
string loadText()
{
    string text = R"(
I have a dream that one day this nation will rise up and live out the true meaning of its creed: "We hold these truths to be self-evident, that all men are created equal."
I have a dream that one day on the red hills of Georgia, the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood.
I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin but by the content of their character.
I have a dream today!
)";
    return text;
}

int main()
{
    // 加载演讲原文
    string text = loadText();

    // 构建Huffman树
    HuffTree huffTree(text);

    // 测试编码
    vector<string> words = {"dream", "equality", "brotherhood", "justice", "freedom"};
    for (const string &word : words)
    {
        string encoded = huffTree.encodeText(word);
        cout << "单词 '" << word << "' 的Huffman编码: " << encoded
             << " (" << encoded.length() << " bits)" << endl;
    }

    // 计算平均编码长度
    double avgLength = huffTree.getAverageCodeLength();
    cout << fixed << setprecision(2);
    cout << "\n平均编码长度: " << avgLength << " bits/character" << endl;

    // 显示部分字符的编码
    cout << "\n部分字符的Huffman编码:" << endl;
    vector<char> chars = {'e', 't', 'a', 'o', 'i', 'n', 's', 'r', 'h', 'l', 'd', 'c'};
    for (char c : chars)
    {
        string code = huffTree.getEncoding(c);
        if (!code.empty())
        {
            cout << c << ": " << code << endl;
        }
    }

    return 0;
}