#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <regex>
#include <chrono>
#include <cmath>
using namespace std;

class Polynomial {
private:
    map<int, float, greater<int>> terms; // 用 map 儲存多項式的項目，key 是指數，value 是係數

public:
    Polynomial() {} // 預設構造函式

    // 添加項目到多項式
    void addterm(int exnt, float coft) {
        if (coft != 0) {  // 只有非零的係數才加入
            terms[exnt] += coft; // 如果已經有該指數的項目，則加上新的係數
            if (terms[exnt] == 0) {
                terms.erase(exnt); // 若係數為零就移除該項
            }
        }
    }

    // 多項式值的計算
    float eval(float x) const {
        float result = 0.0f; // 初始化結果為 0
        for (const auto& term : terms) { // 遍歷所有項目
            result += term.second * pow(x, term.first); // 根據 x 值和指數計算每項的值
        }
        return result; // 回傳計算結果
    }

    // 多項式的輸出運算子
    friend ostream& operator<<(ostream& os, const Polynomial& p) {
        bool first = true; // 用來判斷是否是第一項，處理正負號
        for (const auto& term : p.terms) { // 依照指數降序遍歷每一項
            if (!first && term.second > 0) os << "+"; // 如果不是第一項且係數為正，則加上 "+" 符號
            if (term.first == 0) {
                os << term.second; // 若是常數項則直接輸出係數
            }
            else if (term.first == 1) {
                os << term.second << "X"; // 若指數為 1，則只輸出 X
            }
            else {
                os << term.second << "X^" << term.first; // 否則顯示 X^指數
            }
            first = false; // 設定第一項為 false，後續處理不再顯示 "+" 符號
        }
        if (p.terms.empty()) os << "0"; // 如果多項式沒有項目，則顯示 0
        return os; // 返回輸出串流
    }

    // 多項式的輸入運算子
    friend istream& operator>>(istream& is, Polynomial& p) {
        string pt;
        getline(is, pt); // 讀取整行輸入作為多項式字串
        p.check(pt); // 呼叫解析函式處理輸入字串
        return is;
    }

    // 多項式的加法
    Polynomial add(const Polynomial& poly) const {
        Polynomial result; // 用來儲存結果的多項式
        for (const auto& term : terms) { // 將當前多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        for (const auto& term : poly.terms) { // 將另一個多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        return result; // 回傳加法結果
    }

    // 多項式的減法
    Polynomial sub(const Polynomial& poly) const {
        Polynomial result; // 用來儲存結果的多項式
        for (const auto& term : terms) { // 將當前多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        for (const auto& term : poly.terms) { // 將另一個多項式的項目減去
            result.addterm(term.first, -term.second); // 減去另一個多項式的項
        }
        return result; // 回傳減法結果
    }

    // 多項式的乘法
    Polynomial mult(const Polynomial& poly) const {
        Polynomial result; // 用來儲存結果的多項式
        for (const auto& term1 : terms) { // 遍歷當前多項式的每一項
            for (const auto& term2 : poly.terms) { // 遍歷另一個多項式的每一項
                int newExponent = term1.first + term2.first; // 新指數是兩項指數相加
                float newCoefficient = term1.second * term2.second; // 新係數是兩項係數相乘
                result.addterm(newExponent, newCoefficient); // 將新項加入結果
            }
        }
        return result; // 回傳乘法結果
    }

    // 從字串解析多項式
    void check(const string& pt) {
        if (pt.empty()) { // 檢查字串是否是空的
            cout << "請檢查格式！" << endl;
            return;
        }

        // 正則表達式，用於匹配各項多項式的項目
        regex termPattern(R"(([+-]?\d*\.?\d*)(X(\^(\d+))?)?)");
        smatch match;
        string::const_iterator searchStart(pt.cbegin()); // 設置搜尋的起始位置

        while (regex_search(searchStart, pt.cend(), match, termPattern)) {
            if (match[0].str().empty()) {
                if (searchStart == pt.cend()) break; // 如果搜尋到的字串為空，則跳出迴圈
                searchStart++; // 否則繼續搜尋
                continue;
            }

            string cost = match[1].str(); // 係數部分
            string expt = match[4].str(); // 指數部分

            // 如果係數部分空，則為 1 或 -1
            float coft = (cost.empty() || cost == "+") ? 1.0f : (cost == "-" ? -1.0f : stof(cost));

            // 指數部分空則默認為 1，若有指數部分不為空則取指數
            int exnt = expt.empty() ? (match[2].str().empty() ? 0 : 1) : stoi(expt);

            // 若是純粹的 X 或 X^n，確保指數部分正確
            if (match[2].str() == "X" && expt.empty()) {
                exnt = 1; // X 代表 X^1
            }

            addterm(exnt, coft); // 將係數和指數添加到多項式

            auto nextStart = match.suffix().first; // 取得下個匹配位置
            if (nextStart == pt.cend()) break; // 如果已經處於字串末尾，跳出迴圈
            searchStart = nextStart; // 更新搜尋起點
        }
    }
};

int main() {
    auto start = chrono::high_resolution_clock::now(); // 開始計時

    cout << "請輸入多項式1: ";
    Polynomial p1;
    cin >> p1;

    cout << "請輸入多項式2: ";
    Polynomial p2;
    cin >> p2;

    cout << "選擇運算式[+, -, *]: ";
    char mod;
    cin >> mod;

    Polynomial result; // 用來儲存計算結果
    switch (mod) {
    case '+': // 加法
        result = p1.add(p2);
        cout << "p1(x) + p2(x) = " << result << endl;
        break;
    case '-': // 減法
        result = p1.sub(p2);
        cout << "p1(x) - p2(x) = " << result << endl;
        break;
    case '*': // 乘法
        result = p1.mult(p2);
        cout << "p1(x) * p2(x) = " << result << endl;
        break;
    default: // 無效的運算符號
        cout << "無效的運算符!" << endl;
        return 1;
    }

    cout << "請輸入 x 的值以評估結果: ";
    float x;
    cin >> x;
    cout << "結果在 x = " << x << " 時的值為: " << result.eval(x) << endl;

    auto end = chrono::high_resolution_clock::now(); // 結束計時
    chrono::duration<double> duration = end - start; // 計算時間差
    cout << "程式執行時間: " << duration.count() << " 秒" << endl;

    return 0;
}
