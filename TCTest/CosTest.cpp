#include <windows.h>
#include "CosExt.h"

// ���˻�����������Կ
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

	// ��ʾ��֤��
	cos.show_auth_code();

	// ������֤�룬���˻���ʼ
	char buffer[1024];
	printf("������֤��:");
	scanf_s("%s", buffer, 1024);
	cos.personalize_new_begin(buffer);

	// ��ʾ���Ǵ�
	while (1)
	{
		int ch = get_input();

		if ((ch == 0x4be0) || (ch == 0x4b00))	// �Ϸ�
			sw = cos.prev_word();
		else if ((ch == 0x4de0) || (ch == 0x4d00))	// �·�
			sw = cos.next_word();
		else if (ch == 0x0d)	// ���
		{
			if (sw != SW_6D83_NO_MORE_DATA)
				printf("\n24�����Ǵ���δ��¼��ϣ������Խ�����\n");
			else
				break;
		}
		else if (ch == 0x1b) {
			cos.cancel();
			return 0;
		}
	}

	// ���� PIN ��
	sw = set_pin(cos);

	if (sw == SW_9000_SUCCESS)
		cos.personalize_new_end();

	return 0;
}

// ���˻����ָ�����Կ
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

	// ��ʾ��֤��
	cos.show_auth_code();

	// ������֤�룬���˻���ʼ
	char buffer[1024];
	printf("������֤��:");
	scanf_s("%s", buffer, 1024);
	cos.personalize_restore_begin(buffer);

	// ��ʾ���Ǵ�
	printf("\n�밴�豸Һ��������ʾ�������Ǵ�\n");

	while (1)
	{
		printf("�������Ǵ�:");
		scanf_s("%s", buffer, 1024);
		sw = cos.enter_secret_word(buffer);
		if (sw == SW_6D83_NO_MORE_DATA)
		{
			// ������Ҫ�ĵ��ʶ��Ѿ�������
			break;
		}
		else if (sw == SW_6D82_GET_INTERACTION_LATER)
		{
			// ����Ҫ�������뵥��
			continue;
		}
		else if (sw == SW_6A80_INVALID_DATA)
		{
			// �û�����ĵ��ʲ�����Ч���ʷ�Χ�ڣ����������Ҫ��������
			printf("��������Ǵ���������������\n");
			continue;
		}
		else
		{
			printf("�յ�����ķ���ֵ: %d\n", sw);
			return -1;
		}
	}

	// ���� PIN ��
	sw = set_pin(cos);

	if (sw == SW_9000_SUCCESS)
		cos.personalize_restore_end();

	return 0;
}

// ��ù̼���Ϣ
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

	// ���� PIN
	cos.change_pin_begin();
	int pin_loop = 1;

	while (1)
	{
		if (pin_loop == 1)
			printf("\n������PIN����������Һ��������ʾ�����֣���Ҫ����6λ����\n");
		else
			printf("\n��ȷ��PIN���ٴ�������PIN\n");

		while (1)
		{
			int ch = get_input();

			if ((ch == 0x48e0) || (ch == 0x4800))	// �Ϸ�
				sw = cos.prev_number();
			else if ((ch == 0x50e0) || (ch == 0x5000))	// �·�
				sw = cos.next_number();
			else if ((ch == 0x4be0) || (ch == 0x4b00) || (ch == 0x08))	// ��ɾ
				sw = cos.left_number();
			else if ((ch == 0x4de0) || (ch == 0x4d00) || (ch == 0x08))	// ����
				sw = cos.right_number();
			else if (ch == 0xd)	// ���
			{
				if (sw != SW_9000_SUCCESS)
					printf("\nPIN���Ȳ���6λ���������Խ�����\n");
				else
				{
					sw = cos.change_pin_end(pin_loop);
					if ((pin_loop == 1) && (sw == 0x6D83))	// ����PINʱ����һ��������ȷ��ɣ�Ӧ�÷���6D83����ʾ��һ������ˣ����ǻ���Ҫ��������ڶ���
					{
						pin_loop = 2;
						break;
					}
					else if (pin_loop == 2)
					{
						if (sw == SW_9000_SUCCESS)	// PIN���Ⱥ��ʣ���������Ҳһ�£�ûë��������ȥ��һ����
						{
							pin_loop = 3;
							break;
						}
						else if (sw == 0x6D83)	// ��������Ĳ�һ�£���Ҫ���´ӵ�һ����������
						{
							printf("\n����PIN���벻һ�£�����������PIN\n");
							pin_loop = 1;
							break;
						}
					}
				}
			}
			else if (ch == 0x1B)	// ǿ���˳�
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
		printf("��ȡ�̼���Ϣʧ��\n");
		return -1;
	}

	if (info.firmware_type != 4)
	{
		// ��ʾ��֤��
		cos.show_auth_code();

		// ������֤�룬���˻���ʼ
		char buffer[1024];
		printf("������֤��:");
		scanf_s("%s", buffer, 1024);

		// ����Ҫ��鷵��ֵ��һ��ʧ�ܣ��Ѿ��� bootloader ��
		cos.back_boot_loader(buffer);
	}
	else
		cos.back_boot_loader(nullptr);

	return 0;
}

// ���ع̼�
int burn_cos(const char* burn_file, bool need_back_boot_loader = true)
{
	if (need_back_boot_loader)
	{
		if (back_boot_loader() != 0)
		{
			printf("���� bootloader ����\n");
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
		printf("δ�ҵ����� bootloader ״̬���豸\n");
		return -1;
	}

	if (!cos.connect(0))
	{
		printf("�����豸����\n");
		return -1;
	}

	FILE* fh = fopen(burn_file, "rb");
	if (fh == nullptr)
	{
		printf("�� %s �ļ�����\n", burn_file);
		return -1;
	}

	fseek(fh, 0, SEEK_END);
	size_t filesize = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	if ((filesize % 512) != 0)
	{
		printf("����ȷ�� COS �ļ�\n");
		return -1;
	}

	char* filedata = (char*)malloc(filesize);
	size_t readsize = fread(filedata, 1, filesize, fh);
	if (readsize != filesize)
	{
		printf("�� %s �ļ�ʧ��\n", burn_file);
		free(filedata);
		return -1;
	}

	printf("�����̼��� %u.%u, �Ƿ����(y/n)? ", filedata[0], filedata[1]);

	char buffer[1024];
	scanf_s("%s", buffer, 1024);
	if ((buffer[0] != 'y') && (buffer[0] != 'Y'))
	{
		printf("�û���ֹ����\n");
		free(filedata);
		return 0;
	}

	printf("��ʼ�����̼�\n");
	for (int i = 0; i < (filesize / 512); i++)
	{
		sw = cos.firmware_upgrade(i, &filedata[i * 512]);
		if (sw != SW_9000_SUCCESS)
		{
			printf("�����̼�ʧ��, sw=%x\n", sw);
			free(filedata);
			return -1;
		}
		printf(".");
	}
	printf("\n");

	// �����鷵��ֵ
	sw = cos.reboot_new_firmware();

	for (i = 0; i < 5; i++)
	{
		if (cos.find() > 0)
			break;
		Sleep(500);
	}

	if (i == 5)
	{
		printf("�����̼�ʧ��, �޷��ҵ��豸\n");
		return -1;
	}

	if (!cos.connect(0))
	{
		printf("�����̼�ʧ��, �޷������豸\n");
		return -1;
	}

	printf("�����̼��ɹ�\n");
	free(filedata);
	return 0;
}

#define upgrade_bin_file "btxon-u1-cos-2.1.bin"
//#define temp_bin_file "btxon-u1-ext-2.0.bin"

int get_upgrade_filelist(uint8_t curver[2], uint8_t tarver[2], string bin_filename, vector<string>& filelist)
{
	char buffer[1024];

	// �����ļ���
	string ext = bin_filename.substr(bin_filename.size() - 4);
	if (ext != ".bin")
		return(-3);		// �ļ�����ʽ����

	string filename = bin_filename.substr(0, bin_filename.size() - 4);
	size_t n;
	for ( n = filename.size() - 1; n >= 0; n--)
	{
		if (((filename[n] < '0') || (filename[n] > '9')) && (filename[n] != '.'))
			break;
	}
	filename = bin_filename.substr(0, n + 1);

	// ���汾
	if (curver[0] > tarver[0])
		return -2;	// �̼����ܽ���

	if (curver[0] == tarver[0])	// ��汾��ͬ
	{
		if (curver[1] > tarver[1])
			return -2;	// �̼����ܽ���

		if (curver[1] == tarver[1])	// С�汾Ҳ��ͬ
			return -1;	// �Ѿ������°汾

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

// �����̼�
int upgrade_cos(char* new_ver)
{
	Cos cos;
	SW sw;

	// ת���汾��
	char buffer[256];
	strcpy_s(buffer, 256, new_ver);

	char* pos = strchr(buffer, '.');
	if (pos == nullptr)
	{
		printf("�汾��ʽ����\n");
		return -1;
	}
	*pos = 0;

	uint8_t tarver[2];
	tarver[0] = atoi(buffer);
	tarver[1] = atoi(pos + 1);

	// ���豸
	if (cos.find() <= 0)
	{
		printf("δ���ֱҶ��豸\n");
		return -1;
	}

	if (!cos.connect(0))
		return -1;

	BX_FIRMWARE_INFO info;
	sw = cos.get_firmware_info(info);
	if (sw != SW_9000_SUCCESS)
	{
		printf("��ȡ�̼���Ϣʧ��\n");
		return -1;
	}

	vector<string> filelist;
	int retcode = get_upgrade_filelist(info.version, tarver, upgrade_bin_file, filelist);
	if (retcode == -1)
	{
		printf("�Ѿ������°汾\n");
		return -1;
	}
	if (retcode == -2)
	{
		printf("�̼����ܽ���\n");
		return -1;
	}
	if (retcode == -3)
	{
		printf("�ļ�����ʽ����\n");
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
