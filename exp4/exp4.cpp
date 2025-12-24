#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip> // 修正：去掉多余的 '<'
using namespace std;
using namespace chrono;

// -------------------------- 1. 边界框数据结构定义 --------------------------
struct BoundingBox
{
    float x1, y1, x2, y2, conf;
    BoundingBox(float x1_, float y1_, float x2_, float y2_, float conf_)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), conf(conf_) {}
};

// -------------------------- 2. 四种排序算法实现 --------------------------
void swap(BoundingBox &a, BoundingBox &b)
{
    BoundingBox temp = a;
    a = b;
    b = temp;
}

void bubbleSort(vector<BoundingBox> &boxes)
{
    int n = boxes.size();
    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            if (boxes[j].conf < boxes[j + 1].conf)
            {
                swap(boxes[j], boxes[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}

int partition(vector<BoundingBox> &boxes, int low, int high)
{
    float pivot = boxes[high].conf;
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (boxes[j].conf >= pivot)
        {
            i++;
            swap(boxes[i], boxes[j]);
        }
    }
    swap(boxes[i + 1], boxes[high]);
    return i + 1;
}

void quickSortRecursive(vector<BoundingBox> &boxes, int low, int high)
{
    if (low < high)
    {
        int pi = partition(boxes, low, high);
        quickSortRecursive(boxes, low, pi - 1);
        quickSortRecursive(boxes, pi + 1, high);
    }
}

void quickSort(vector<BoundingBox> &boxes)
{
    if (!boxes.empty())
        quickSortRecursive(boxes, 0, boxes.size() - 1);
}

void merge(vector<BoundingBox> &boxes, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<BoundingBox> L(boxes.begin() + left, boxes.begin() + mid + 1);
    vector<BoundingBox> R(boxes.begin() + mid + 1, boxes.begin() + right + 1);
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i].conf >= R[j].conf)
            boxes[k++] = L[i++];
        else
            boxes[k++] = R[j++];
    }
    while (i < n1)
        boxes[k++] = L[i++];
    while (j < n2)
        boxes[k++] = R[j++];
}

void mergeSortRecursive(vector<BoundingBox> &boxes, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSortRecursive(boxes, left, mid);
        mergeSortRecursive(boxes, mid + 1, right);
        merge(boxes, left, mid, right);
    }
}

void mergeSort(vector<BoundingBox> &boxes)
{
    if (!boxes.empty())
        mergeSortRecursive(boxes, 0, boxes.size() - 1);
}

void heapify(vector<BoundingBox> &boxes, int n, int i)
{
    int largest = i, l = 2 * i + 1, r = 2 * i + 2;
    if (l < n && boxes[l].conf > boxes[largest].conf)
        largest = l;
    if (r < n && boxes[r].conf > boxes[largest].conf)
        largest = r;
    if (largest != i)
    {
        swap(boxes[i], boxes[largest]);
        heapify(boxes, n, largest);
    }
}

void heapSort(vector<BoundingBox> &boxes)
{
    int n = boxes.size();
    if (n <= 1)
        return;
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(boxes, n, i);
    for (int i = n - 1; i > 0; i--)
    {
        swap(boxes[0], boxes[i]);
        heapify(boxes, i, 0);
    }
}

// -------------------------- 3. 数据生成模块 --------------------------
float randomFloat(float min, float max, mt19937 &gen)
{
    uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

vector<BoundingBox> generateBBoxes(int mode, int count)
{
    vector<BoundingBox> boxes;
    random_device rd;
    mt19937 gen(rd());

    if (mode == 0)
    { // 随机分布
        for (int i = 0; i < count; i++)
        {
            float x1 = randomFloat(0, 640 - 80, gen);
            float y1 = randomFloat(0, 480 - 80, gen);
            float w = randomFloat(20, 80, gen);
            float h = randomFloat(20, 80, gen);
            boxes.emplace_back(x1, y1, x1 + w, y1 + h, randomFloat(0.1f, 1.0f, gen));
        }
    }
    else
    { // 聚集分布
        vector<pair<float, float>> centers = {{100, 100}, {540, 100}, {100, 380}, {540, 380}};
        uniform_int_distribution<int> centerDist(0, 3); // 修正：使用 gen 的分布器
        for (int i = 0; i < count; i++)
        {
            int centerIdx = centerDist(gen); // 替代 rand() % 4
            float cx = centers[centerIdx].first;
            float cy = centers[centerIdx].second;
            float x1 = randomFloat(cx - 50, cx - 20, gen);
            float y1 = randomFloat(cy - 50, cy - 20, gen);
            float w = randomFloat(20, 60, gen);
            float h = randomFloat(20, 60, gen);
            boxes.emplace_back(x1, y1, x1 + w, y1 + h, randomFloat(0.1f, 1.0f, gen));
        }
    }
    return boxes;
}

// -------------------------- 4. NMS 算法 --------------------------
float calculateIoU(const BoundingBox &a, const BoundingBox &b)
{
    float interX1 = max(a.x1, b.x1);
    float interY1 = max(a.y1, b.y1);
    float interX2 = min(a.x2, b.x2);
    float interY2 = min(a.y2, b.y2);
    float interArea = max(0.0f, interX2 - interX1) * max(0.0f, interY2 - interY1);
    if (interArea == 0)
        return 0.0f;
    float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
    float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);
    return interArea / (areaA + areaB - interArea);
}

vector<BoundingBox> basicNMS(vector<BoundingBox> sortedBoxes, float iouThreshold = 0.5f)
{
    vector<BoundingBox> result;
    while (!sortedBoxes.empty())
    {
        BoundingBox top = sortedBoxes[0];
        result.push_back(top);
        vector<BoundingBox> temp;
        for (size_t i = 1; i < sortedBoxes.size(); ++i)
        {
            if (calculateIoU(top, sortedBoxes[i]) < iouThreshold)
                temp.push_back(sortedBoxes[i]);
        }
        sortedBoxes = move(temp); // 可加 move 提升效率（可选）
    }
    return result;
}

// -------------------------- 5. 性能测试 --------------------------
typedef void (*SortFunc)(vector<BoundingBox> &);

double testSortPerformance(SortFunc sortFunc, vector<BoundingBox> boxes)
{
    auto start = high_resolution_clock::now();
    sortFunc(boxes);
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

void fullPerformanceTest()
{
    vector<string> sortNames = {"冒泡排序", "快速排序", "归并排序", "堆排序"};
    vector<SortFunc> sortFuncs = {bubbleSort, quickSort, mergeSort, heapSort};
    vector<int> dataModes = {0, 1};
    vector<string> modeNames = {"随机分布", "聚集分布"};
    vector<int> dataSizes = {100, 1000, 5000, 10000};
    const int testTimes = 5;

    cout << "==================== 排序算法性能测试（NMS前置排序） ====================\n";
    cout << "测试配置：IoU阈值=0.5，每种情况测试" << testTimes << "次取平均值\n";
    cout << setw(12) << "数据规模" << setw(12) << "数据分布" << setw(12) << "排序算法" << setw(12) << "平均耗时(ms)\n";
    cout << "-------------------------------------------------------------------------\n";

    for (int size : dataSizes)
    {
        for (int mode : dataModes)
        {
            vector<BoundingBox> originalBoxes = generateBBoxes(mode, size);
            for (size_t i = 0; i < sortFuncs.size(); ++i)
            {
                double totalTime = 0.0;
                for (int t = 0; t < testTimes; ++t)
                {
                    totalTime += testSortPerformance(sortFuncs[i], originalBoxes);
                }
                cout << setw(12) << size
                     << setw(12) << modeNames[mode]
                     << setw(12) << sortNames[i]
                     << setw(12) << fixed << setprecision(3) << (totalTime / testTimes) << "\n";
            }
        }
    }

    cout << "\n==================== NMS整体性能测试（快速排序+NMS） ====================\n";
    cout << setw(12) << "数据规模" << setw(12) << "数据分布" << setw(12) << "NMS耗时(ms)" << setw(12) << "保留框数量\n";
    cout << "-------------------------------------------------------------------------\n";
    for (int size : dataSizes)
    {
        for (int mode : dataModes)
        {
            vector<BoundingBox> boxes = generateBBoxes(mode, size);
            auto start = high_resolution_clock::now();
            quickSort(boxes);
            vector<BoundingBox> nmsResult = basicNMS(boxes);
            auto end = high_resolution_clock::now();
            double nmsTime = duration<double, milli>(end - start).count();
            cout << setw(12) << size
                 << setw(12) << modeNames[mode]
                 << setw(12) << fixed << setprecision(3) << nmsTime
                 << setw(12) << nmsResult.size() << "\n";
        }
    }
}

// -------------------------- 主函数 --------------------------
int main()
{
    fullPerformanceTest();
    return 0;
}