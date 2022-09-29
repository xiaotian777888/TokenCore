#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <io.h>
#include <CosBase.h>
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

// ���ͷ 4be0, �Ҽ�ͷ 4de0, �ϼ�ͷ 48e0, �¼�ͷ 50e0
// ���ͷ 4b00, �Ҽ�ͷ 4d00, �ϼ�ͷ 4800, �¼�ͷ 5000 (С����)
// backspace 8, �س� 0x0d

int get_input()
{
	union
	{
		unsigned char ch[4];
		int  value;
	} input;

	input.value = 0;

	input.ch[0] = (unsigned char)_getch();
	if ((input.ch[0] == 0xE0) || (input.ch[0] == 0))
		input.ch[1] = (unsigned char)_getch();

	return input.value;
}

void binout(unsigned char* data, int len)
{
	for (int i = 0; i < len; i++)
		printf("%02X", data[i]);
	printf("\n");
}

// ���� PIN ��(�ӳ���)
int set_pin(Cos& cos)
{
	SW sw;

	// ���� PIN
	cos.set_pin_begin();
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
					sw = cos.set_pin_end(pin_loop);
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

	return sw;
}

// ��֤ PIN (�ӳ���)
int verify_pin(Cos& cos)
{
	SW sw;

	BX_PIN_INFO pin_info;
	sw = cos.get_pin_info(pin_info);
	if (pin_info.SecureState == C_AC_USER)
		printf("�Ѿ���¼��\n");
	else if (pin_info.CurrentRetry == 0)
	{
		printf("�豸�Ѿ�����\n");
		return -1;
	}
	else
	{
		if (pin_info.CurrentRetry != pin_info.MaxRetry)
		{
			// ��ʾ��֤��
			cos.show_auth_code();

			// ������֤�룬���˻���ʼ
			char buffer[1024];
			printf("��������֤��:");
			scanf_s("%s", buffer, 1024);

			// ��Ҫ��֤PIN
			sw = cos.verify_pin_begin(buffer);
		}
		else
		{
			// ��Ҫ��֤PIN
			printf("������PIN��");
			sw = cos.verify_pin_begin(nullptr);
		}

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
				printf("\n");
				if (sw != SW_9000_SUCCESS)
					printf("\nPIN���Ȳ���6λ���������Խ�����\n");
				else
				{
					sw = cos.verify_pin_end();
					if (sw != SW_9000_SUCCESS)
					{
						printf("��֤PIN��ʧ��\n");
						return -1;
					}
					printf("��֤PIN�ɹ�\n");
					break;
				}
			}
			else if (ch == 0x1B)	// ǿ���˳�
			{
				cos.cancel();
				return -1;
			}
		}
	}

	return 0;
}

// ��ȡǮ����ַ
int get_wallet_address(const CoinType& coin_type, unsigned long index, char* wallet_address)
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

	sw = cos.get_payment_address(coin_type.major, coin_type.minor, coin_type.chain_id, index, wallet_address, true);
	if (sw != SW_9000_SUCCESS)
	{
		printf("ȡǮ����ַʧ��\n");
		return -1;
	}

	return 0;
}

// ��ȡǮ����ַ
int dump_wallet_address(const CoinType& coin_type)
{
	char wallet_address[256];
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

	for (unsigned int i = 0; i < 64; i++)
	{
		sw = cos.get_payment_address(coin_type.major, coin_type.minor, coin_type.chain_id, i, wallet_address, true);
		if (sw != SW_9000_SUCCESS)
		{
			printf("ȡǮ����ַʧ��\n");
			return -1;
		}
		printf("Ǯ����ַ[%02d]: %s\n", i, wallet_address);
	}

	return 0;
}

