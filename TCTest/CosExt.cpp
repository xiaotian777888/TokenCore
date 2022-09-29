#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <conio.h>
#include <io.h>
#include <CosBase.h>
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

// 左箭头 4be0, 右箭头 4de0, 上箭头 48e0, 下箭头 50e0
// 左箭头 4b00, 右箭头 4d00, 上箭头 4800, 下箭头 5000 (小键盘)
// backspace 8, 回车 0x0d

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

// 设置 PIN 码(子程序)
int set_pin(Cos& cos)
{
	SW sw;

	// 设置 PIN
	cos.set_pin_begin();
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
					sw = cos.set_pin_end(pin_loop);
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

	return sw;
}

// 验证 PIN (子程序)
int verify_pin(Cos& cos)
{
	SW sw;

	BX_PIN_INFO pin_info;
	sw = cos.get_pin_info(pin_info);
	if (pin_info.SecureState == C_AC_USER)
		printf("已经登录了\n");
	else if (pin_info.CurrentRetry == 0)
	{
		printf("设备已经锁死\n");
		return -1;
	}
	else
	{
		if (pin_info.CurrentRetry != pin_info.MaxRetry)
		{
			// 显示验证码
			cos.show_auth_code();

			// 输入验证码，个人化开始
			char buffer[1024];
			printf("请输入验证码:");
			scanf_s("%s", buffer, 1024);

			// 需要验证PIN
			sw = cos.verify_pin_begin(buffer);
		}
		else
		{
			// 需要验证PIN
			printf("请输入PIN码");
			sw = cos.verify_pin_begin(nullptr);
		}

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
				printf("\n");
				if (sw != SW_9000_SUCCESS)
					printf("\nPIN长度不足6位数，不可以结束！\n");
				else
				{
					sw = cos.verify_pin_end();
					if (sw != SW_9000_SUCCESS)
					{
						printf("验证PIN码失败\n");
						return -1;
					}
					printf("验证PIN成功\n");
					break;
				}
			}
			else if (ch == 0x1B)	// 强行退出
			{
				cos.cancel();
				return -1;
			}
		}
	}

	return 0;
}

// 获取钱包地址
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
		printf("取钱包地址失败\n");
		return -1;
	}

	return 0;
}

// 获取钱包地址
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
			printf("取钱包地址失败\n");
			return -1;
		}
		printf("钱包地址[%02d]: %s\n", i, wallet_address);
	}

	return 0;
}

