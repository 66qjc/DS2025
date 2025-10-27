#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

class Complex
{
private:
    double real;
    double imag;

public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    double getReal() const { return real; }

    double getImag() const { return imag; }

    void setReal(double r) { real = r; }

    void setImag(double i) { imag = i; }

    double modulus() const { return sqrt(real * real + imag * imag); }

    bool operator==(const Complex &other) const
    {
        return (real == other.real) && (imag == other.imag);
    }

    bool operator!=(const Complex &other) const
    {
        return !(*this == other);
    }

    friend ostream &operator<<(ostream &os, const Complex &c)
    {
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

vector<Complex> generateRandomComplexVector(int size, double minVal, double maxVal)
{
    vector<Complex> vec;
    static bool seeded = false;
    if (!seeded)
    {
        srand(time(0));
        seeded = true;
    }

    for (int i = 0; i < size; ++i)
    {
        double real = minVal + (maxVal - minVal) * rand() / RAND_MAX;
        double imag = minVal + (maxVal - minVal) * rand() / RAND_MAX;
        vec.push_back(Complex(real, imag));
    }
    return vec;
}

void shuffleVector(vector<Complex> &vec)
{
    for (size_t i = vec.size() - 1; i > 0; --i)
    {
        size_t j = rand() % (i + 1);
        swap(vec[i], vec[j]);
    }
}

size_t findComplex(const vector<Complex> &vec, const Complex &target)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i] == target)
        {
            return i;
        }
    }
    return -1;
}

void insertComplex(vector<Complex> &vec, size_t pos, const Complex &c)
{
    if (pos <= vec.size())
    {
        vec.insert(vec.begin() + pos, c);
    }
}

void deleteComplex(vector<Complex> &vec, size_t pos)
{
    if (pos < vec.size())
    {
        vec.erase(vec.begin() + pos);
    }
}

void uniqueVector(vector<Complex> &vec)
{
    vector<Complex> temp;
    for (const auto &c : vec)
    {
        if (findComplex(temp, c) == -1)
        {
            temp.push_back(c);
        }
    }
    vec = temp;
}

void bubbleSort(vector<Complex> &vec)
{
    size_t n = vec.size();
    for (size_t i = 0; i < n - 1; ++i)
    {
        for (size_t j = 0; j < n - i - 1; ++j)
        {
            if (vec[j].modulus() > vec[j + 1].modulus() ||
                (vec[j].modulus() == vec[j + 1].modulus() && vec[j].getReal() > vec[j + 1].getReal()))
            {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
}

void merge(vector<Complex> &vec, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<Complex> L(n1), R(n2);
    for (int i = 0; i < n1; ++i)
        L[i] = vec[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = vec[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i].modulus() < R[j].modulus() ||
            (L[i].modulus() == R[j].modulus() && L[i].getReal() <= R[j].getReal()))
        {
            vec[k++] = L[i++];
        }
        else
        {
            vec[k++] = R[j++];
        }
    }

    while (i < n1)
        vec[k++] = L[i++];
    while (j < n2)
        vec[k++] = R[j++];
}

void mergeSort(vector<Complex> &vec, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(vec, left, mid);
        mergeSort(vec, mid + 1, right);
        merge(vec, left, mid, right);
    }
}

vector<Complex> rangeSearch(const vector<Complex> &vec, double m1, double m2)
{
    vector<Complex> result;
    for (const auto &c : vec)
    {
        double mod = c.modulus();
        if (mod >= m1 && mod < m2)
        {
            result.push_back(c);
        }
    }
    return result;
}

void printVector(const vector<Complex> &vec, const string &msg = "")
{
    if (!msg.empty())
        cout << msg << endl;
    for (const auto &c : vec)
    {
        cout << c << " ";
    }
    cout << endl
         << endl;
}

int main()
{
    int size = 10;
    vector<Complex> complexVec = generateRandomComplexVector(size, -10, 10);
    printVector(complexVec, "初始随机复数向量：");

    shuffleVector(complexVec);
    printVector(complexVec, "置乱后的向量：");

    if (!complexVec.empty())
    {
        Complex target = complexVec[0];
        size_t pos = findComplex(complexVec, target);
        if (pos != -1)
        {
            cout << "查找 " << target << " 的位置：" << pos << endl
                 << endl;
        }
    }

    Complex insertVal(100, 200);
    insertComplex(complexVec, 2, insertVal);
    stringstream ss;
    ss << "插入 " << insertVal << " 后的向量：";
    printVector(complexVec, ss.str());

    if (complexVec.size() > 3)
    {
        deleteComplex(complexVec, 3);
        printVector(complexVec, "删除索引3的元素后的向量：");
    }

    uniqueVector(complexVec);
    printVector(complexVec, "唯一化后的向量：");

    int testSize = 10000;
    vector<Complex> testVec = generateRandomComplexVector(testSize, -100, 100);

    vector<Complex> sortedVec = testVec;
    mergeSort(sortedVec, 0, sortedVec.size() - 1);

    vector<Complex> reversedVec = sortedVec;
    reverse(reversedVec.begin(), reversedVec.end());

    vector<Complex> shuffledVec = testVec;
    shuffleVector(shuffledVec);

    clock_t start, end;
    double duration;

    vector<Complex> temp = sortedVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序（顺序）耗时：" << duration << "秒" << endl;

    temp = shuffledVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序（乱序）耗时：" << duration << "秒" << endl;

    temp = reversedVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序（逆序）耗时：" << duration << "秒" << endl
         << endl;

    temp = sortedVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序（顺序）耗时：" << duration << "秒" << endl;

    temp = shuffledVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序（乱序）耗时：" << duration << "秒" << endl;

    temp = reversedVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    duration = (double)(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序（逆序）耗时：" << duration << "秒" << endl
         << endl;

    double m1 = 2.0, m2 = 5.0;
    vector<Complex> rangeResult = rangeSearch(sortedVec, m1, m2);
    ss.clear();
    ss << "模介于[" << m1 << ", " << m2 << ")的元素：";
    printVector(rangeResult, ss.str());

    return 0;
}