#pragma once

#ifdef WIN32
#ifdef COSBASE_EXPORTS
#define COSBASE_API __declspec(dllexport)
#else
#define COSBASE_API __declspec(dllimport)
#endif
#else
#ifdef COSBASE_EXPORTS
#define COSBASE_API __attribute__((visibility("default")))
#else
#define COSBASE_API __attribute__((visibility("default")))
#endif
#endif //WIN32

#include <vector>
using namespace std;

#ifndef __BX_DEVICE_H__
#pragma pack(push, 1)
typedef struct BX_FIRMWARE_INFO
{
	unsigned char ver_st;                      // 本结构体的版本
	unsigned char firmware_type;               // 固件类型: 1=boot, 2=loader, 3=app, 4=app-ext
	unsigned char flag;                        // 标志，00 = 正式发布版, ff = 内部开发测试版
	unsigned char model;                       // 设备类型， 01 = 一代, 02 = 二代Key，BLE

	unsigned char sn[16];                      // 芯片的SN
	unsigned char vid[2];                      // vendor id
	unsigned char pid[2];                      // product id
	unsigned char version[2];                  // COS版本号
	unsigned char vendor_name[8];              //
	unsigned char product_name[16];            //
}BX_FIRMWARE_INFO;

typedef struct BX_PIN_INFO
{
	unsigned char MaxRetry;
	unsigned char CurrentRetry;
	unsigned char UpdateCounter;
	unsigned char SecureState;
}BX_PIN_INFO;
#pragma pack(pop)
#endif

#ifndef __BX_CONST_H__
//=================================================
// SW
//=================================================
#define SW_9000_SUCCESS						0x9000          // OK
#define SW_6400_FAILED                      0x6400			// 6400 : 执行错误，一般用于内部错误。
#define SW_6700_INVALID_LC  				0x6700			// 6700 : 长度错误
#define SW_6881_INVALID_CHANNEL 			0x6881			// 6881 : 不支持的逻辑通道
#define SW_6982_ACCESS_DENIED				0x6982			// 6982 : 安全状态不满足（验证失败）
#define SW_6983_PIN_LOCKED					0x6983			// 6983 : PIN已经锁死
#define SW_6985_CONDITION_NOT_FULFILL		0x6985			// 6985 : 使用条件不满足。
#define SW_6986_NO_EF_SELECTED				0x6986			// 6986 : 当前没有选择EF。
#define SW_6A80_INVALID_DATA				0x6A80			// 6A80 : 命令数据域的参数不正确
#define SW_6A81_NOT_SUPPORT					0x6A81			// 6A81 : 功能不支持。
#define SW_6A82_FILE_NOT_EXISTS				0x6A82			// 6A82 : 文件不存在。
#define SW_6A84_NOT_ENOUGH_SPACE			0x6A84			// 6A84 : 空间不足
#define SW_6A86_INVALID_P1P2				0x6A86			// 6A86 : APDU指令中的参数P1或P2错误
#define SW_6A89_FILE_EXISTS					0x6A89			// 6A89 : 目标文件已经存在。
#define SW_6A8A_DF_NAME_EXISTS				0x6A8A			// 6A8A : 目标DF名称已经存在。
#define SW_6D00_INVALID_INS					0x6D00			// 6D00 : APDU指令无法识别。
#define SW_6E00_INVALID_CLA					0x6E00			// 6E00 : APDU指令中的CLA错误
#define SW_6D80_COMM_FAILED					0x6D80			// 6D80 : 传输过程中发生了错误
#define SW_6D81_NO_FREE_CHANNEL				0x6D81			// 6D81 : 无法打开逻辑通道，所有逻辑通道均已被占用
#define SW_6D82_GET_INTERACTION_LATER		0x6D82	        // 6D82 : COS已经收到指令，正在处理，请稍后取处理结果
#define SW_6D83_NO_MORE_DATA         		0x6D83          // 6D83 : 没有更多数据了（交互可以结束了）
#define SW_6D84_USER_CANCELED				0x6D84			// 6D84 : 用户取消了操作
#define SW_6D85_TIMEOUT						0x6D85			// 6D85 : 交互操作超时
#define SW_6D86_BUSY						0x6D86			// 6D86 : 设备忙（例如正在进行交互）
#define SW_6D87_TOO_MUCH_TO_SHOWN			0x6D87			// 6D87 : 要显示的内容超出设备极限
#define SW_6D88_INVALID_DATA	            0x6D88			// 6D88 : 交易报文无法解析，可能格式有误
#define SW_6D89_BIP39_CHECKSUM_NOT_MATCH	0x6D89			// 6D89 : 助记词校验合不匹配

//=================================================
// device life cycle
//=================================================
#define BX_LCS_BLANK            0xFF
#define BX_LCS_NOINIT           0xEF
#define BX_LCS_USER             0x5F
#define BX_LCS_LOCKED           0x00

//=================================================
// coin type
//=================================================
#define COIN_BTC				0x00
#define COIN_BTC_TEST           0x01

#endif

#define C_AC_NOBODY 0x00
#define C_AC_USER 0x02
#define C_AC_EVERYONE 0xFF

#ifndef bxStr
#	if defined(UNICODE) || defined(_UNICODE)
typedef std::basic_string<wchar_t>	bxStr;
#	else
typedef std::basic_string<char>		bxStr;
#	endif
#endif  //bxStr

typedef void(*cos_device_insert_callback)(void* param, const bxStr& node_path, unsigned short vid, unsigned short pid, const bxStr& dev_path);
typedef void(*cos_device_remove_callback)(void* param, const bxStr& node_path, unsigned short vid, unsigned short pid, const bxStr& dev_path);

class COSBASE_API Cos
{
public:
	bool is_connected;
	bool is_open_channel;
	void* cosdev;

	Cos();
	~Cos();

	const bxStr& getConnectedDevice() const;

	void reset();			// 强制复位
	int find();				// 查找设备
	int find_bootloader();	// 查找处于bootloader状态的设备
	bool connect(int index);	// 连接设备
	void disconnect();			// 断开连接
	bool open_channel();	// 打开通道
	bool close_channel();	// 关闭通道
	void dump_device();		// 显示连接的设备
	unsigned short show_auth_code();	// 显示验证码
	unsigned short personalize_new_begin(const char* auth_code);	// 个人化：创建主密钥开始
	unsigned short personalize_new_end();					// 个人化：创建主密钥完成
	unsigned short personalize_restore_begin(const char* auth_code);	// 个人化：恢复密钥开始
	unsigned short personalize_restore_end();				// 个人化：恢复密钥完成
	unsigned short next_word();			// 下翻
	unsigned short prev_word();			// 上翻
	unsigned short set_pin_begin();		// 设置 PIN 码
	unsigned short set_pin_end(int n);	// 设置 PIN 码输入完成, n是第几次
	unsigned short prev_number();		// 上一个数字
	unsigned short next_number();		// 下一个数字
	unsigned short left_number();		// 左移
	unsigned short right_number();		// 右移
	unsigned short get_lifecycle(unsigned char& lcs);	// 获取生命周期
	unsigned short get_firmware_info(BX_FIRMWARE_INFO& info);	// 获取固件信息
	unsigned short get_pin_info(BX_PIN_INFO& info);				// 获取 PIN 码信息
	unsigned short verify_pin_begin(const char* authcode);		// 验证 PIN 码开始
	unsigned short verify_pin_end();							// 验证 PIN 码完成
	unsigned short get_payment_address(unsigned short coin_major, unsigned short coin_minor, unsigned long coin_chain_id, unsigned long key_index, char* wallet_address, bool is_show);	// 获得钱包地址
	unsigned short get_random(unsigned short len, vector<unsigned char>& ret_data);	// 获得随机数
	unsigned short cancel();		// 取消一次交互
	unsigned short enter_secret_word(const char* mnemonic);		// 输入一个助记词
	unsigned short sign_transaction(unsigned short coin_major, unsigned short coin_minor, unsigned long coin_chain_id, unsigned long key_index, const unsigned char* tx_data, int tx_size, char* sig_data, int& sig_size);	// 签名一次交易
	unsigned short sign_transaction_end(const char* authcode, char* sig_data, int& sig_size);		// 签名交易完成
	unsigned short change_pin_begin();		// 修改 PIN 码
	unsigned short change_pin_end(int n);	// 修改 PIN 码输入完成, n是第几次
	unsigned short back_boot_loader(const char* auth_code);	// 回 bootloader
	unsigned short firmware_upgrade(const int index, const char* data);	// 升级固件
	unsigned short reboot_new_firmware();	// 重启新固件

	unsigned short get_public_key(unsigned short coin_major, unsigned short coin_minor, unsigned long coin_chain_id, unsigned long key_index, char* keybuff, int& key_len);	// 获得钱包公钥

	unsigned short sign_authcode(const char* code, char* sig_data, int& sig_size);	// 签名验证码

	unsigned short sign_data(unsigned short coin_major, unsigned short coin_minor, unsigned long coin_chain_id, unsigned long key_index, const unsigned char* tx_data, int tx_size, char* sig_data, int& sig_size);	// 签名一次交易
	unsigned short sign_data_end(const char* authcode, char* sig_data, int& sig_size);		// 签名交易完成

	static bool start_device_monitor(void* hwnd, void* param, cos_device_insert_callback on_device_insert, cos_device_remove_callback on_device_remove);
	static void stop_device_monitor();

private:
	bxStr connected_device;
};
