#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <regex>
#include <chrono>
using namespace std;

class Polynomial {
private://用 map 儲存多項式的項目，key 是指數，value 是係數
    map<int, float, greater<int>> terms; // 依照指數降序排列

public:
    // 預設構造函式
    Polynomial() {}
    // 添加項目到多項式
    void addterm(int exnt, float coft) {
        if (coft != 0) {  //只有非零的係數才加入
            terms[exnt] += coft;//如果已經有該指數的項目，則加上新的係數
            if (terms[exnt] == 0) {
                terms.erase(exnt); //若係數為零就移除該項
            }
        }
    }

    void print() const {//多項式輸出函式
        bool first = true;  // 用來判斷是否是第一項，處理正負號
        for (const auto& term : terms) {  //依照指數降序遍歷每一項
            if (!first && term.second > 0) cout << "+";//如果不是第一項且係數為正，則加上"+"符號
            if (term.first == 0) {
                cout << term.second; //若是常數項則直接輸出係數
            }
            else if (term.first == 1) {
                cout << term.second << "X";//若指數為1，則只輸出X
            }
            else {
                cout << term.second << "X^" << term.first;//否則顯示 X^指數
            }
            first = false;//設定第一項為false，後續處理不再顯示"+"符號
        }
        if (terms.empty()) cout << "0";//如果多項式沒有項目，則顯示0
        cout << endl;
    }

    // 多項式的加法
    Polynomial add(const Polynomial& poly) const {
        Polynomial result;//用來儲存結果的多項式
        for (const auto& term : terms) {//將當前多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        for (const auto& term : poly.terms) {//將另一個多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        return result;//回傳加法結果
    }

    // 多項式的減法
    Polynomial sub(const Polynomial& poly) const {
        Polynomial result;//用來儲存結果的多項式
        for (const auto& term : terms) {//將當前多項式的項目加入結果
            result.addterm(term.first, term.second);
        }
        for (const auto& term : poly.terms) {//將另一個多項式的項目減去
            result.addterm(term.first, -term.second);//減去另一個多項式的項
        }
        return result;//回傳減法結果
    }

    // 多項式的乘法
    Polynomial mult(const Polynomial& poly) const {
        Polynomial result;  // 用來儲存結果的多項式
        for (const auto& term1 : terms) {  // 遍歷當前多項式的每一項
            for (const auto& term2 : poly.terms) {  // 遍歷另一個多項式的每一項
                int newExponent = term1.first + term2.first;  // 新指數是兩項指數相加
                float newCoefficient = term1.second * term2.second;  // 新係數是兩項係數相乘
                result.addterm(newExponent, newCoefficient);  // 將新項加入結果
            }
        }
        return result;//回傳乘法結果
    }

    // 從字串解析多項式
    void check(const string& pt) {
        if (pt.empty()) {  //檢查字串是否是空的(是就離開)
            cout << "請檢查格式！" << endl;
            return;
        }

        // 正則表達式，用於匹配各項多項式的項目[by Chatgpt]
        regex termPattern(R"(([+-]?\d*\.?\d*)(X(\^(\d+))?)?)");
        smatch match;
        string::const_iterator searchStart(pt.cbegin());//設置搜尋的起始位置

        // 使用正則表達式匹配每一項
        while (regex_search(searchStart, pt.cend(), match, termPattern)) {
            if (match[0].str().empty()) {
                if (searchStart == pt.cend()) break;//如果搜尋到的字串為空，則跳出迴圈
                searchStart++;//否則繼續搜尋
                continue;
            }

            //分類係數和指數
            string cost = match[1].str(); // 係數部分
            string expt = match[4].str(); // 指數部分

            // 如果係數部分空，則為 1 或 -1
            float coft = (cost.empty() || cost == "+") ? 1.0f : (cost == "-" ? -1.0f : stof(cost));

            // 指數部分空則默認為1，若有指數部分不為空則取指數
            int exnt = expt.empty() ? (match[2].str().empty() ? 0 : 1) : stoi(expt);

            // 若是純粹的X或X^n，確保指數部分正確
            if (match[2].str() == "X" && expt.empty()) {
                exnt = 1; // X 代表 X^1
            }

            // 將係數和指數添加到多項式
            addterm(exnt, coft);

            auto nextStart = match.suffix().first; //取得下個匹配位置
            if (nextStart == pt.cend()) break; //如果已經處於字串末尾，跳出迴圈
            searchStart = nextStart; //更新搜尋起點
        }
    }
};

int main() {
    auto start = chrono::high_resolution_clock::now();
    cout << "請輸入多項式1: ";
    string pt1;
    getline(cin, pt1);//讀取p1輸入的多項式
    Polynomial p1;
    p1.check(pt1);//解析p1並且儲存多項式

    cout << "請輸入多項式2: ";
    string pt2;
    getline(cin, pt2);//讀取p2輸入的多項式
    Polynomial p2;
    p2.check(pt2);//解析p2並且儲存多項式

    cout << "選擇運算式[+, -, *]: ";
    char mod;
    cin >> mod;//讀取運算符號

    Polynomial result;//用來儲存計算結果
    switch (mod)//根據運算符來判斷運算方式 
    {
    case '+'://加法
        result = p1.add(p2);
        cout << "p1(x) + p2(x) = ";
        break;
    case '-'://減法
        result = p1.sub(p2);
        cout << "p1(x) - p2(x) = ";
        break;
    case '*':  //乘法
        result = p1.mult(p2);
        cout << "p1(x) * p2(x) = ";
        break;
    default://無效的運算符號
        cout << "無效的運算符!" << endl;
        return 1;
    }
    result.print();//輸出計算結果
    auto end = chrono::high_resolution_clock::now();  // 記錄結束時間
    chrono::duration<double> duration = end - start;  // 計算時間差
    cout << "程式執行時間: " << duration.count() << " 秒" << endl;  // 輸出執行時間
    return 0;
}