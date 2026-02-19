#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>

using namespace std;
namespace fs = std::filesystem;
void unPack();
void ClearOTA();
void ClearImages();
void Start();
int enterChoice() {
    cout << "\n輸入你的選擇\n"
        << "1.解包全量包\n"
        << "2.清空ota文件\n"
        << "3.清空images文件\n"
        << "4.開始刷機\n"
        << "5.END\n"
        << "?";
    int choice;
    cin >> choice;
    return choice;
}

fs::path otaDir = L"ota";
fs::path imagesDir = L"images";
fs::path toolsDir = L"tools";
fs::path payloadPath = toolsDir / L"payload-dumper-go.exe";
fs::path fastbootPath = toolsDir / L"fastboot.exe";
fs::path adbPath = toolsDir / L"fastboot.exe";

int main()
{
    if (!fs::exists(imagesDir)) {
        fs::create_directory(imagesDir);
    }
    if (!fs::exists(otaDir)) {
        fs::create_directory(otaDir);
    }
    if (!fs::exists(toolsDir)) {
        fs::create_directory(toolsDir);
    }
    cout << "---------ONEPLUS 15 升降級工具---------" << endl;
    int choice;

    while ((choice = enterChoice()) != 5) {
        switch (choice) {
        case 1:
            unPack();
            break;
        case 2:
            ClearOTA();
            break;
        case 3:
            ClearImages();
            break;
        case 4:
            Start();
            break;
        default:
            break;
        }
    }

}
void unPack() {
    //解包zip.
    fs::path updatezipPath = otaDir / L"update.zip";
    fs::path sevenZipPath = toolsDir / L"7z.exe";
    //7z x "update.zip" "payload.bin" -o"ota"
    wstring UnpackZipCmd = L"\"\"" + sevenZipPath.wstring() + L"\" x \"" + updatezipPath.wstring() + L"\" \"payload.bin\" -o\"" + otaDir.wstring() + L"\"\"";
    int ZipResult = _wsystem(UnpackZipCmd.c_str());
    if (ZipResult == 0) {
        cout << "\n解壓縮update.zip成功\n";
        system("pause");
    }
    else {
        cout << "ERROR:系統找不到 /ota/update.zip \n";
        system("pause");
        return;
    }
    fs::path BinPath = otaDir / L"payload.bin";
    if (!fs::exists(BinPath)) {
        cout << "提取payload.bin失敗\n" << "確認檔案是否存在\n";
        system("pause");
        return;
    }
    //解包bin
    //payload.exe -o "輸出" "輸入"
    cout << "\n開始解包payload.bin\n";
    wstring ExtractBincmd = L"\"\"" + payloadPath.wstring() + L"\" -o \"" + imagesDir.wstring() + L"\" \"" + BinPath.wstring() + L"\"\"";
    int BinResult = _wsystem(ExtractBincmd.c_str());
    if (BinResult == 0) {
        cout << "\n解包payload.bin成功\n";
    }
}
void ClearOTA() {
    if (fs::exists(otaDir)) {
        char choice;
        cout << "確認刪除?[y/n]\n";
        cin >> choice;
        if (choice == 'y') {
            uintmax_t count = fs::remove_all(otaDir);
            fs::create_directory(otaDir);
            cout << "刪除完成，案任意鍵返回\n";
            system("pause");
            /*for (auto &entry : fs::directory_iterator(otaDir)) {
                try {
                    fs::remove(entry.path());
                }
                catch (fs::filesystem_error) {
                    cout << "無法刪除 " << entry.path().filename() << endl;
                }
            }*/
        }
        else {
            cout << "已取消刪除\n";
            return;
        }
    }
    else {
        fs::create_directory(otaDir);
    }
}
void ClearImages() {
    if (fs::exists(imagesDir)) {
        char choice;
        cout << "確認刪除?[y/n]\n";
        cin >> choice;
        if (choice == 'y') {
            uintmax_t count = fs::remove_all(imagesDir);
            fs::create_directory(imagesDir);
            cout << "刪除完成，案任意鍵返回\n";
            system("pause");
        }
        else {
            cout << "已取消刪除\n";
            return;
        }
    }
    else {
        fs::create_directory(imagesDir);
    }
}
void Start() {
    int choice;
    do {
        cout << "1.升/降級?\n"
            << "2.返回\n"
            << "?";
        cin >> choice;
        if (choice == 2) {
            return;
        }
    } while (choice < 1 || choice > 1);

    vector<wstring> cowPartitions = {
    L"system",
    L"system_dlkm",
    L"system_ext",
    L"vendor",
    L"product",
    L"odm",
    L"my_bigball",
    L"my_carrier",
    L"my_engineering",
    L"my_heytap",
    L"my_manifest",
    L"my_product",
    L"my_region",
    L"my_stock",
    L"odm_dlkm",
    L"vendor_dlkm"
    };
    vector<wstring> fwPartitions = {
        //實體底層
    L"vbmeta", L"vbmeta_system", L"vbmeta_vendor", L"vendor_boot", L"init_boot", L"boot",
    L"recovery", L"bluetooth", L"cpucp", L"cpucp_dtb", L"dsp", L"dtbo", L"engineering_cdt",
    L"featenabler", L"oplus_sec", L"shrm", L"splash", L"uefi", L"aop", L"aop_config",
    L"devcfg", L"hyp", L"imagefv", L"keymaster", L"oplusstanvbk", L"qupfw", L"tz",
    L"uefisecapp", L"abl", L"xbl", L"xbl_config", L"xbl_ramdump"
    };
    vector<wstring> sysPartitions = {
        //動態
    L"my_bigball", L"my_carrier", L"my_engineering", L"my_heytap", L"my_manifest",
    L"my_product", L"my_region", L"my_stock", L"odm", L"product", L"system",
    L"system_dlkm", L"system_ext", L"vendor", L"vendor_dlkm"
    };
    //刷入img檔案
    fs::path modem_path = imagesDir / L"modem.img";
    _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash modem_a \"" + modem_path.wstring() + L"\"\"").c_str());
    _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash modem_b \"" + modem_path.wstring() + L"\"\"").c_str());
    cout << "\n正在進入Fastboot模式，請勿動手機和電腦\n";
    _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" reboot fastboot\"").c_str());
    cout << "\n請等待10秒，否則會出錯\n";
    this_thread::sleep_for(chrono::seconds(10));
    //bin\fastboot delete-logical-partition system_a-cow
    cout << "\n[處理中]清除COW暫存空間\n";
    for (const wstring& s : cowPartitions) {
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_a-cow\"").c_str());
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_b-cow\"").c_str());
    }
    cout << "COW 暫存空間清除完成\n";
    //bin\fastboot flash vbmeta_a images\vbmeta.img
    //bin\fastboot flash vbmeta_b images\vbmeta.img
    for (const wstring& s : fwPartitions) {
        fs::path imgPath = imagesDir / (s + L".img");
        if (fs::exists(imgPath)) {
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_a \"" + imgPath.wstring() + L"\"\"").c_str());
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_b \"" + imgPath.wstring() + L"\"\"").c_str());
        }
        else {
            if (s == L"cpucp_dtb" || s == L"oplusstanvbk") {
                wcout << L"\n[略過] 未找到可選分區 " << s << L".img，已自動忽略\n";
            }
        }
    }
    //bin\fastboot flash my_bigball images\my_bigball.img
    for (const wstring& s : sysPartitions) {
        fs::path imgPath = imagesDir / (s + L".img");
        if (fs::exists(imgPath)) {
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L" \"" + imgPath.wstring() + L"\"\"").c_str());
        }
    }
    cout << "\n請在手機選擇語言，並選擇格式化數據後重啟\n";
    cout << "刷機完成，按任意鍵返回主頁\n";
    system("pause");
}
