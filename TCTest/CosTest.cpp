#include <windows.h>
#include "CosExt.h"

// 个人化：创建主密钥
int personal_new()
{
	Cos cos;
	SW sw;

	if (cos.find() <= 0)
		return -1;

	if (!cos.connect(0))
		return -1;

	if (!cos.open_channel())
		return -1;

	// 显示验证码
	cos.show_auth_code();

	// 输入验证码，个人化开始
	char buffer[1024];
	printf("输入验证码:");
	scanf_s("%s", buffer, 1024);
	cos.personalize_new_begin(buffer);

	// 显示助记词
	while (1)
	{
		int ch = get_input();

		if ((ch == 0x4be0) || (ch == 0x4b00))	// 上翻
			sw = cos.prev_word();
		else if ((ch == 0x4de0) || (ch == 0x4d00))	// 下翻
			sw = cos.next_word();
		else if (ch == 0x0d)	// 完成
		{
			if (sw != SW_6D83_NO_MORE_DATA)
				printf("\n24个助记词尚未记录完毕，不可以结束！\n");
			else
				break;
		}
		else if (ch == 0x1b) {
			cos.cancel();
			return 0;
		}
	}

	// 设置 PIN 码
	sw = set_pin(cos);

	if (sw == SW_9000_SUCCESS)
		cos.personalize_new_end();

	return 0;
}

// 个人化：恢复主密钥
int personal_restore()
{
	Cos cos;
	SW sw;

	if (cos.find() <= 0)
		return -1;

	if (!cos.connect(0))
		return -1;

	if (!cos.open_channel())
		return -1;

	// 显示验证码
	cos.show_auth_code();

	// 输入验证码，个人化开始
	char buffer[1024];
	printf("输入验证码:");
	scanf_s("%s", buffer, 1024);
	cos.personalize_restore_begin(buffer);

	// 显示助记词
	printf("\n请按设备液晶屏上提示输入助记词\n");

	while (1)
	{
		printf("输入助记词:");
		scanf_s("%s", buffer, 1024);
		sw = cos.enter_secret_word(buffer);
		if (sw == SW_6D83_NO_MORE_DATA)
		{
			// 所有需要的单词都已经输入了
			break;
		}
		else if (sw == SW_6D82_GET_INTERACTION_LATER)
		{
			// 还需要继续输入单词
			continue;
		}
		else if (sw == SW_6A80_INVALID_DATA)
		{
			// 用户输入的单词不在有效单词范围内，这个单词需要重新输入
			printf("输入的助记词有误，请重新输入\n");
			continue;
		}
		else
		{
			printf("收到错误的返回值: %d\n", sw);
			return -1;
		}
	}

	// 设置 PIN 码
	sw = set_pin(cos);

	if (sw == SW_9000_SUCCESS)
		cos.personalize_restore_end();

	return 0;
}

// 获得固件信息
int get_firmware_info()
{
	Cos cos;

	if (cos.find() <= 0)
		return -1;

	if (!cos.connect(0))
		return -1;

	/*
	if (!cos.open_channel())
		return -1;
	*/

	unsigned char lcs = 0;
	cos.get_lifecycle(lcs);

	BX_FIRMWARE_INFO info;
	cos.get_firmware_info(info);

	printf(" - Life Cycle: %02x\n", lcs);
	printf(" - VID: %02x%02x\n", info.vid[0], info.vid[1]);
	printf(" - PID: %02x%02x\n", info.pid[0], info.pid[1]);
	printf(" - SN: ");
	for (int i = 0; i < 16; i++)
		printf("%02x", info.sn[i]);
	printf("\n");
	printf(" - Version: %02x.%02x\n", info.version[0], info.version[1]);
	printf(" - Model: %02x\n", info.model);
	printf(" - Vender Name : %s\n", (const char*)info.vendor_name);
	printf(" - Product Name: %s\n", (const char*)info.product_name);

	return 0;
}

int change_pin()
{
	Cos cos;
	SW sw;

	if (cos.find() <= 0)
		return -1;

	if (!cos.connect(0))
		return -1;

	if (!cos.open_channel())
		return -1;

	if (verify_pin(cos) != 0)
		return -1;

	// 设置 PIN
	cos.change_pin_begin();
	int pin_loop = 1;

	while (1)
	{
		if (pin_loop == 1)
			printf("\n请设置PIN，按键控制液晶屏上显示的数字（需要至少6位数）\n");
		else
			printf("\n请确认PIN，再次输入新PIN\n");

		while (1)
		{
			int ch = get_input();

			if ((ch == 0x48e0) || (ch == 0x4800))	// 上翻
				sw = cos.prev_number();
			else if ((ch == 0x50e0) || (ch == 0x5000))	// 下翻
				sw = cos.next_number();
			else if ((ch == 0x4be0) || (ch == 0x4b00) || (ch == 0x08))	// 回删
				sw = cos.left_number();
			else if ((ch == 0x4de0) || (ch == 0x4d00) || (ch == 0x08))	// 右移
				sw = cos.right_number();
			else if (ch == 0xd)	// 完成
			{
				if (sw != SW_9000_SUCCESS)
					printf("\nPIN长度不足6位数，不可以结束！\n");
				else
				{
					sw = cos.change_pin_end(pin_loop);
					if ((pin_loop == 1) && (sw == 0x6D83))	// 设置PIN时，第一遍输入正确完成，应该返回6D83，表示这一轮完成了，但是还需要继续输入第二遍
					{
						pin_loop = 2;
						break;
					}
					else if (pin_loop == 2)
					{
						if (sw == SW_9000_SUCCESS)	// PIN长度合适，两次输入也一致，没毛病，可以去下一步了
						{
							pin_loop = 3;
							break;
						}
						else if (sw == 0x6D83)	// 两次输入的不一致，需要重新从第一遍输入来过
						{
							printf("\n两次PIN输入不一致，请重新设置PIN\n");
							pin_loop = 1;
							break;
						}
					}
				}
			}
			else if (ch == 0x1B)	// 强行退出
			{
				cos.cancel();
				return 0;
			}
		}
		if (pin_loop == 3)
			break;
	}

	return 0;
}

int back_boot_loader()
{
	Cos cos;

	if (cos.find() <= 0)
		return -1;

	if (!cos.connect(0))
		return -1;

	/*
	if (!cos.open_channel())
		return -1;
	*/

	BX_FIRMWARE_INFO info;
	SW sw = cos.get_firmware_info(info);
	if (sw != SW_9000_SUCCESS)
	{
		printf("获取固件信息失败\n");
		return -1;
	}

	if (info.firmware_type != 4)
	{
		// 显示验证码
		cos.show_auth_code();

		// 输入验证码，个人化开始
		char buffer[1024];
		printf("输入验证码:");
		scanf_s("%s", buffer, 1024);

		// 不需要检查返回值，一定失败，已经回 bootloader 了
		cos.back_boot_loader(buffer);
	}
	else
		cos.back_boot_loader(nullptr);

	return 0;
}

// 下载固件
int burn_cos(const char* burn_file, bool need_back_boot_loader = true)
{
	if (need_back_boot_loader)
	{
		if (back_boot_loader() != 0)
		{
			printf("返回 bootloader 错误\n");
			return -1;
		}
	}

	Cos cos;
	SW sw;

	int i;
	for (i = 0; i < 5; i++)
	{
		if (cos.find_bootloader() > 0)
			break;
		Sleep(500);
	}

	if (i == 5)
	{
		printf("未找到处于 bootloader 状态的设备\n");
		return -1;
	}

	if (!cos.connect(0))
	{
		printf("连接设备错误\n");
		return -1;
	}

	FILE* fh = fopen(burn_file, "rb");
	if (fh == nullptr)
	{
		printf("打开 %s 文件错误\n", burn_file);
		return -1;
	}

	fseek(fh, 0, SEEK_END);
	size_t filesize = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	if ((filesize % 512) != 0)
	{
		printf("不正确的 COS 文件\n");
		return -1;
	}

	char* filedata = (char*)malloc(filesize);
	size_t readsize = fread(filedata, 1, filesize, fh);
	if (readsize != filesize)
	{
		printf("读 %s 文件失败\n", burn_file);
		free(filedata);
		return -1;
	}

	printf("升级固件到 %u.%u, 是否继续(y/n)? ", filedata[0], filedata[1]);

	char buffer[1024];
	scanf_s("%s", buffer, 1024);
	if ((buffer[0] != 'y') && (buffer[0] != 'Y'))
	{
		printf("用户终止升级\n");
		free(filedata);
		return 0;
	}

	printf("开始升级固件\n");
	for (int i = 0; i < (filesize / 512); i++)
	{
		sw = cos.firmware_upgrade(i, &filedata[i * 512]);
		if (sw != SW_9000_SUCCESS)
		{
			printf("升级固件失败, sw=%x\n", sw);
			free(filedata);
			return -1;
		}
		printf(".");
	}
	printf("\n");

	// 无需检查返回值
	sw = cos.reboot_new_firmware();

	for (i = 0; i < 5; i++)
	{
		if (cos.find() > 0)
			break;
		Sleep(500);
	}

	if (i == 5)
	{
		printf("升级固件失败, 无法找到设备\n");
		return -1;
	}

	if (!cos.connect(0))
	{
		printf("升级固件失败, 无法连接设备\n");
		return -1;
	}

	printf("升级固件成功\n");
	free(filedata);
	return 0;
}

#define upgrade_bin_file "btxon-u1-cos-2.1.bin"
//#define temp_bin_file "btxon-u1-ext-2.0.bin"

int get_upgrade_filelist(uint8_t curver[2], uint8_t tarver[2], string bin_filename, vector<string>& filelist)
{
	char buffer[1024];

	// 处理文件名
	string ext = bin_filename.substr(bin_filename.size() - 4);
	if (ext != ".bin")
		return(-3);		// 文件名格式错误

	string filename = bin_filename.substr(0, bin_filename.size() - 4);
	size_t n;
	for ( n = filename.size() - 1; n >= 0; n--)
	{
		if (((filename[n] < '0') || (filename[n] > '9')) && (filename[n] != '.'))
			break;
	}
	filename = bin_filename.substr(0, n + 1);

	// 检查版本
	if (curver[0] > tarver[0])
		return -2;	// 固件不能降级

	if (curver[0] == tarver[0])	// 大版本相同
	{
		if (curver[1] > tarver[1])
			return -2;	// 固件不能降级

		if (curver[1] == tarver[1])	// 小版本也相同
			return -1;	// 已经是最新版本

		sprintf_s(buffer, 1024, "%s%d.%d.bin", filename.c_str(), tarver[0], tarver[1]);
		filelist.push_back(buffer);
		return 0;
	}

	for (uint8_t i = curver[0] + 1; i <= tarver[0]; i++)
	{
		sprintf_s(buffer, 1024, "%s%d.0.bin", filename.c_str(), i);
		filelist.push_back(buffer);
	}

	sprintf_s(buffer, 1024, "%s%d.%d.bin", filename.c_str(), tarver[0], tarver[1]);
	filelist.push_back(buffer);
	return 0;
}

// 升级固件
int upgrade_cos(char* new_ver)
{
	Cos cos;
	SW sw;

	// 转化版本号
	char buffer[256];
	strcpy_s(buffer, 256, new_ver);

	char* pos = strchr(buffer, '.');
	if (pos == nullptr)
	{
		printf("版本格式不对\n");
		return -1;
	}
	*pos = 0;

	uint8_t tarver[2];
	tarver[0] = atoi(buffer);
	tarver[1] = atoi(pos + 1);

	// 找设备
	if (cos.find() <= 0)
	{
		printf("未发现币盾设备\n");
		return -1;
	}

	if (!cos.connect(0))
		return -1;

	BX_FIRMWARE_INFO info;
	sw = cos.get_firmware_info(info);
	if (sw != SW_9000_SUCCESS)
	{
		printf("获取固件信息失败\n");
		return -1;
	}

	vector<string> filelist;
	int retcode = get_upgrade_filelist(info.version, tarver, upgrade_bin_file, filelist);
	if (retcode == -1)
	{
		printf("已经是最新版本\n");
		return -1;
	}
	if (retcode == -2)
	{
		printf("固件不能降级\n");
		return -1;
	}
	if (retcode == -3)
	{
		printf("文件名格式错误\n");
		return -1;
	}

	for (size_t i = 0; i < filelist.size(); i++)
	{
		if ((info.pid[0] == 0) && (info.pid[1] == 0) && (i==0))
			burn_cos(filelist[i].c_str(), false);
		else
			burn_cos(filelist[i].c_str());
	}

	return 0;
}

void CosTest()
{
	get_firmware_info();
	//dump_wallet_address(gCoin["tBTC"].type);
	//personal_new();
	//personal_restore();
	//change_pin();
	//burn_cos("bitcoin-os-1.1.bin");
	//upgrade_cos("2.1");
}
