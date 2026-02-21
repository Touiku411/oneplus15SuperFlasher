#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;
void Environment();
void bootTobootloader();
void bootToRecovery();
void unPack();
void ClearOTA();
void ClearImages();
void Start();
void shengjianji();
void jiuzhuan();
int enterChoice() {
    cout << "       ---------ONEPLUS 15 升降級工具---------" << endl;
    cout << "\033[1;31m";
    cout << "====================================================\n"
        << "!僅限[一加15]使用，若使用其他機型導致變磚，一概不負責!\n"
        << "!請將ota包更改名稱為update.zip，並放入ota資料夾下!\n"
        << "!!!!!!!!!!!!!!!!!!!!!!\n"
        << "!!!需解鎖bootloader!!!\n"
        << "!!!需解鎖bootloader!!!\n"
        << "!!!需解鎖bootloader!!!\n"
        << "!!!!!!!!!!!!!!!!!!!!!!\n"
        << "====================================================\n";
    cout << "\033[0m\n";
    cout << "=> 輸入你的選擇\n"
        << "1.測試環境\n"
        << "2.解包全量包\n"
        << "3.清空ota資料夾\n"
        << "4.清空images資料夾\n"
        << "5.開始刷機\n"
        << "6.END\n"
        << "? ";
    int choice;
    cin >> choice;
    return choice;
}
int enterChoice2() {
    system("cls");
    cout << "一些實用指令\n" << endl;
    cout << "1.adb devices\n"
        << "2.fastboot devices\n"
        << "3.重啟至 bootloader\n"
        << "4.重啟至 Recovery\n"
        << "5.返回\n"
        << "? ";
    int choice;
    cin >> choice;
    return choice;
}

fs::path otaDir = L"ota";
fs::path imagesDir = L"images";
fs::path toolsDir = L"tools";
fs::path payloadPath = toolsDir / L"payload-dumper-go.exe";
fs::path fastbootPath = toolsDir / L"fastboot.exe";
fs::path adbPath = toolsDir / L"adb.exe";

int main()
{
    system("chcp 65001 > nul");
    if (!fs::exists(imagesDir))
        fs::create_directory(imagesDir);
    if (!fs::exists(otaDir))
        fs::create_directory(otaDir);
    if (!fs::exists(toolsDir))
        fs::create_directory(toolsDir);
    int choice;

    while ((choice = enterChoice()) != 6) {
        switch (choice) {
        case 1:
            Environment();
            break;
        case 2:
            unPack();
            break;
        case 3:
            ClearOTA();
            break;
        case 4:
            ClearImages();
            break;
        case 5:
            Start();
            break;
        default:
            break;
        }
    }

}
void Environment() {
    int choice;
    while ((choice = enterChoice2()) != 5) {
        switch (choice) {
        case 1:
            cout << "\n若什麼都沒顯示代表adb沒識別到裝置，請先嘗試安裝驅動\n\n";
            _wsystem((L"\"\"" + adbPath.wstring() + L"\" devices\"").c_str());
            system("pause");
            break;
        case 2:
            cout << "\n若什麼都沒顯示代表fastboot沒識別到裝置，請確認已處於fastboot或bootloader模式下，或者請先嘗試安裝驅動\n\n";
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" devices\"").c_str());
            system("pause");
            break;
        case 3:
            bootTobootloader();
            break;
        case 4:
            bootToRecovery();
            break;
        default:
            break;
        }
    }
    system("cls");
}
void bootTobootloader() {
    wstring cmd1 = L"\"\"" + adbPath.wstring() + L"\" reboot bootloader 2>nul\"";
    wstring cmd2 = L"\"\"" + fastbootPath.wstring() + L"\" reboot bootloader\"";
    int result1 = _wsystem(cmd1.c_str());
    if (result1 != 0) {
        int result2 = _wsystem(cmd2.c_str());
        if (result2 != 0) {
            cout << "失敗\n";
        }
        else {
            cout << "\n正在重啟至bootloader\n\n";
        }
    }
    else {
        cout << "\n正在重啟至bootloader\n\n";
    }
    system("pause");
    system("cls");
}
void bootToRecovery() {
    wstring cmd1 = L"\"\"" + adbPath.wstring() + L"\" reboot recovery 2>nul\"";
    wstring cmd2 = L"\"\"" + fastbootPath.wstring() + L"\" reboot recovery\"";
    int result1 = _wsystem(cmd1.c_str());
    if (result1 != 0) {
        int result2 = _wsystem(cmd2.c_str());
        if (result2 != 0) {
            cout << "失敗\n";
        }
        else {
            cout << "\n正在重啟至Recovery\n\n";
        }
    }
    else {
        cout << "\n正在重啟至Recovery\n\n";
    }
    system("pause");
    system("cls");
}
void unPack() {
    //解包zip.
    fs::path updatezipPath = otaDir / L"update.zip";
    fs::path sevenZipPath = toolsDir / L"7z.exe";
    if (!fs::exists(updatezipPath)) {
        cout << "ERROR:系統找不到 /ota/update.zip \n";
        cout << "請確認是否已將檔案改名為update.zip並放入ota資料夾下 \n";
        system("pause");
        system("cls");
        return;
    }
    //7z x "update.zip" "payload.bin" -o"ota"
    wstring UnpackZipCmd = L"\"\"" + sevenZipPath.wstring() + L"\" x \"" + updatezipPath.wstring() + L"\" \"payload.bin\" -o\"" + otaDir.wstring() + L"\"\"";
    int ZipResult = _wsystem(UnpackZipCmd.c_str());
    if (ZipResult == 0) {
        cout << "\n解壓縮update.zip成功\n"
            << "按任意鍵繼續提取payload.bin....\n\n";
        system("pause > nul");
    }
    else {
        cout << "\nERROR: 解壓縮失敗，請確認 update.zip 是否損壞。\n";
        system("pause > nul");
        system("cls");
        return;
    }
    fs::path BinPath = otaDir / L"payload.bin";
    if (!fs::exists(BinPath)) {
        cout << "提取payload.bin失敗\n" << "確認檔案是否存在\n";
        system("pause");
        system("cls");
        return;
    }
    //解包bin
    //payload.exe -o "輸出" "輸入"
    cout << "\n開始解包payload.bin\n\n";
    wstring ExtractBincmd = L"\"\"" + payloadPath.wstring() + L"\" -o \"" + imagesDir.wstring() + L"\" \"" + BinPath.wstring() + L"\"\"";
    int BinResult = _wsystem(ExtractBincmd.c_str());
    if (BinResult == 0) {
        cout << "\n解包payload.bin成功\n";
    }
    system("pause");
    system("cls");
}
void ClearOTA() {
    if (fs::exists(otaDir)) {
        char choice;
        cout << "確認刪除?[y/n]";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            uintmax_t count = fs::remove_all(otaDir);
            fs::create_directory(otaDir);
            cout << "刪除完成，案任意鍵返回\n";
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
        }
        system("pause");
        system("cls");
    }
    else {
        fs::create_directory(otaDir);
    }
}
void ClearImages() {
    if (fs::exists(imagesDir)) {
        char choice;
        cout << "確認刪除?[y/n]";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            uintmax_t count = fs::remove_all(imagesDir);
            fs::create_directory(imagesDir);
            cout << "刪除完成，案任意鍵返回\n";
        }
        else {
            cout << "已取消刪除\n";
        }
    }
    else {
        fs::create_directory(imagesDir);
    }
    system("pause");
    system("cls");
}
void Start() {
    int choice;
    do {
        cout << "1.升/降級?\n"
            << "2.救磚(限同版本刷入)\n"
            << "3.返回\n"
            << "?";
        cin >> choice;
        if (choice == 3) {
            system("cls");
            return;
        }
    } while (choice < 1 || choice > 2);
    switch (choice) {
    case 1:
        shengjianji();
        break;
    case 2:
        jiuzhuan();
        break;
    default:
        break;
    }

}
void shengjianji() {
    vector<wstring> cowPartitions = {
    L"system", L"system_dlkm", L"system_ext", L"vendor",L"product",
    L"odm",L"my_bigball",L"my_carrier",L"my_engineering",L"my_heytap",
    L"my_manifest",L"my_product",L"my_region", L"my_stock", L"odm_dlkm", L"vendor_dlkm"
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
    if (fs::exists(modem_path)) {
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash modem_a \"" + modem_path.wstring() + L"\"\"").c_str());
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash modem_b \"" + modem_path.wstring() + L"\"\"").c_str());
    }
    cout << "\n正在進入Fastboot模式，請勿動手機和電腦\n";
    _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" reboot fastboot\"").c_str());
    cout << "\n請等待10秒...\n";
    this_thread::sleep_for(chrono::seconds(10));
    //bin\fastboot delete-logical-partition system_a-cow
    for (const wstring& s : cowPartitions) {
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_a-cow\"").c_str());
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_b-cow\"").c_str());
    }
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
    system("pause > nul");
    system("cls");
}
void jiuzhuan() {
    vector<wstring> preFwPartitions = {
        L"vbmeta", L"vbmeta_system", L"vbmeta_vendor", L"vendor_boot", L"init_boot",
        L"boot", L"recovery", L"modem", L"bluetooth", L"cpucp", L"cpucp_dtb",
        L"dsp", L"dtbo", L"engineering_cdt", L"featenabler", L"oplus_sec",
        L"shrm", L"splash", L"uefi"
    };
    vector<wstring> cowPartitions = {
        L"system", L"system_dlkm", L"system_ext", L"vendor", L"product", L"odm",
        L"my_bigball", L"my_carrier", L"my_engineering", L"my_heytap", L"my_manifest",
        L"my_product", L"my_region", L"my_stock", L"odm_dlkm", L"vendor_dlkm"
    };
    vector<wstring> postFwPartitions = {
        L"aop", L"aop_config", L"devcfg", L"hyp", L"imagefv", L"keymaster",
        L"oplusstanvbk", L"qupfw", L"tz", L"uefisecapp", L"abl", L"xbl",
        L"xbl_config", L"xbl_ramdump"
    };
    vector<wstring> sysPartitions = {
        L"my_bigball", L"my_carrier", L"my_engineering", L"my_heytap", L"my_manifest",
        L"my_product", L"my_region", L"my_stock", L"odm", L"product", L"system",
        L"system_dlkm", L"system_ext", L"vendor", L"vendor_dlkm"
    };
    for (const wstring& s : preFwPartitions) {
        fs::path imgPath = imagesDir / (s + L".img");
        if (fs::exists(imgPath)) {
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_a \"" + imgPath.wstring() + L"\"\"").c_str());
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_b \"" + imgPath.wstring() + L"\"\"").c_str());
        }
        else {
            if (s == L"cpucp_dtb") {
                wcout << L"\n[略過] 未找到可選分區 " << s << L".img，已自動忽略\n";
            }
        }
    }
    cout << "\n正在進入Fastboot模式，請勿動手機和電腦\n";
    _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" reboot fastboot\"").c_str());
    cout << "\n請等待10秒...\n";
    this_thread::sleep_for(chrono::seconds(10));
    for (const wstring& s : cowPartitions) {
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_a-cow\"").c_str());
        _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" delete-logical-partition " + s + L"_b-cow\"").c_str());
    }
    for (const wstring& s : postFwPartitions) {
        fs::path imgPath = imagesDir / (s + L".img");
        if (fs::exists(imgPath)) {
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_a \"" + imgPath.wstring() + L"\"\"").c_str());
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L"_b \"" + imgPath.wstring() + L"\"\"").c_str());
        }
        else {
            if (s == L"oplusstanvbk") {
                wcout << L"\n[略過] 未找到可選分區 " << s << L".img，已自動忽略\n";
            }
        }
    }
    for (const wstring& s : sysPartitions) {
        fs::path imgPath = imagesDir / (s + L".img");
        if (fs::exists(imgPath)) {
            _wsystem((L"\"\"" + fastbootPath.wstring() + L"\" flash " + s + L" \"" + imgPath.wstring() + L"\"\"").c_str());
        }
    }
    cout << "\n請在手機選擇語言，並選擇格式化數據後重啟\n";
    cout << "刷機完成，按任意鍵返回主頁\n";
    system("pause > nul");
    system("cls");
}