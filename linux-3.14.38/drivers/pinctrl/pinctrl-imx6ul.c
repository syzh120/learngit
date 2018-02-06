/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 * imx6ul平台的pin控制器驱动
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-imx.h"

// 定义了imx6ul平台的pin脚号
enum imx6ul_pads {
	MX6UL_PAD_RESERVE0 = 0,
	MX6UL_PAD_RESERVE1 = 1,
	MX6UL_PAD_RESERVE2 = 2,
	MX6UL_PAD_RESERVE3 = 3,
	MX6UL_PAD_RESERVE4 = 4,
	MX6UL_PAD_RESERVE5 = 5,
	MX6UL_PAD_RESERVE6 = 6,
	MX6UL_PAD_RESERVE7 = 7,
	MX6UL_PAD_RESERVE8 = 8,
	MX6UL_PAD_RESERVE9 = 9,
	MX6UL_PAD_RESERVE10 = 10,
	MX6UL_PAD_SNVS_TAMPER4 = 11,
	MX6UL_PAD_RESERVE12 = 12,
	MX6UL_PAD_RESERVE13 = 13,
	MX6UL_PAD_RESERVE14 = 14,
	MX6UL_PAD_RESERVE15 = 15,
	MX6UL_PAD_RESERVE16 = 16,
	MX6UL_PAD_JTAG_MOD = 17,
	MX6UL_PAD_JTAG_TMS = 18,
	MX6UL_PAD_JTAG_TDO = 19,
	MX6UL_PAD_JTAG_TDI = 20,
	MX6UL_PAD_JTAG_TCK = 21,
	MX6UL_PAD_JTAG_TRST_B = 22,
	MX6UL_PAD_GPIO1_IO00 = 23,
	MX6UL_PAD_GPIO1_IO01 = 24,
	MX6UL_PAD_GPIO1_IO02 = 25,
	MX6UL_PAD_GPIO1_IO03 = 26,
	MX6UL_PAD_GPIO1_IO04 = 27,
	MX6UL_PAD_GPIO1_IO05 = 28,
	MX6UL_PAD_GPIO1_IO06 = 29,
	MX6UL_PAD_GPIO1_IO07 = 30,
	MX6UL_PAD_GPIO1_IO08 = 31,
	MX6UL_PAD_GPIO1_IO09 = 32,
	MX6UL_PAD_UART1_TX_DATA = 33,
	MX6UL_PAD_UART1_RX_DATA = 34,
	MX6UL_PAD_UART1_CTS_B = 35,
	MX6UL_PAD_UART1_RTS_B = 36,
	MX6UL_PAD_UART2_TX_DATA = 37,
	MX6UL_PAD_UART2_RX_DATA = 38,
	MX6UL_PAD_UART2_CTS_B = 39,
	MX6UL_PAD_UART2_RTS_B = 40,
	MX6UL_PAD_UART3_TX_DATA = 41,
	MX6UL_PAD_UART3_RX_DATA = 42,
	MX6UL_PAD_UART3_CTS_B = 43,
	MX6UL_PAD_UART3_RTS_B = 44,
	MX6UL_PAD_UART4_TX_DATA = 45,
	MX6UL_PAD_UART4_RX_DATA = 46,
	MX6UL_PAD_UART5_TX_DATA = 47,
	MX6UL_PAD_UART5_RX_DATA = 48,
	MX6UL_PAD_ENET1_RX_DATA0 = 49,
	MX6UL_PAD_ENET1_RX_DATA1 = 50,
	MX6UL_PAD_ENET1_RX_EN = 51,
	MX6UL_PAD_ENET1_TX_DATA0 = 52,
	MX6UL_PAD_ENET1_TX_DATA1 = 53,
	MX6UL_PAD_ENET1_TX_EN = 54,
	MX6UL_PAD_ENET1_TX_CLK = 55,
	MX6UL_PAD_ENET1_RX_ER = 56,
	MX6UL_PAD_ENET2_RX_DATA0 = 57,
	MX6UL_PAD_ENET2_RX_DATA1 = 58,
	MX6UL_PAD_ENET2_RX_EN = 59,
	MX6UL_PAD_ENET2_TX_DATA0 = 60,
	MX6UL_PAD_ENET2_TX_DATA1 = 61,
	MX6UL_PAD_ENET2_TX_EN = 62,
	MX6UL_PAD_ENET2_TX_CLK = 63,
	MX6UL_PAD_ENET2_RX_ER = 64,
	MX6UL_PAD_LCD_CLK = 65,
	MX6UL_PAD_LCD_ENABLE = 66,
	MX6UL_PAD_LCD_HSYNC = 67,
	MX6UL_PAD_LCD_VSYNC = 68,
	MX6UL_PAD_LCD_RESET = 69,
	MX6UL_PAD_LCD_DATA00 = 70,
	MX6UL_PAD_LCD_DATA01 = 71,
	MX6UL_PAD_LCD_DATA02 = 72,
	MX6UL_PAD_LCD_DATA03 = 73,
	MX6UL_PAD_LCD_DATA04 = 74,
	MX6UL_PAD_LCD_DATA05 = 75,
	MX6UL_PAD_LCD_DATA06 = 76,
	MX6UL_PAD_LCD_DATA07 = 77,
	MX6UL_PAD_LCD_DATA08 = 78,
	MX6UL_PAD_LCD_DATA09 = 79,
	MX6UL_PAD_LCD_DATA10 = 80,
	MX6UL_PAD_LCD_DATA11 = 81,
	MX6UL_PAD_LCD_DATA12 = 82,
	MX6UL_PAD_LCD_DATA13 = 83,
	MX6UL_PAD_LCD_DATA14 = 84,
	MX6UL_PAD_LCD_DATA15 = 85,
	MX6UL_PAD_LCD_DATA16 = 86,
	MX6UL_PAD_LCD_DATA17 = 87,
	MX6UL_PAD_LCD_DATA18 = 88,
	MX6UL_PAD_LCD_DATA19 = 89,
	MX6UL_PAD_LCD_DATA20 = 90,
	MX6UL_PAD_LCD_DATA21 = 91,
	MX6UL_PAD_LCD_DATA22 = 92,
	MX6UL_PAD_LCD_DATA23 = 93,
	MX6UL_PAD_NAND_RE_B = 94,
	MX6UL_PAD_NAND_WE_B = 95,
	MX6UL_PAD_NAND_DATA00 = 96,
	MX6UL_PAD_NAND_DATA01 = 97,
	MX6UL_PAD_NAND_DATA02 = 98,
	MX6UL_PAD_NAND_DATA03 = 99,
	MX6UL_PAD_NAND_DATA04 = 100,
	MX6UL_PAD_NAND_DATA05 = 101,
	MX6UL_PAD_NAND_DATA06 = 102,
	MX6UL_PAD_NAND_DATA07 = 103,
	MX6UL_PAD_NAND_ALE = 104,
	MX6UL_PAD_NAND_WP_B = 105,
	MX6UL_PAD_NAND_READY_B = 106,
	MX6UL_PAD_NAND_CE0_B = 107,
	MX6UL_PAD_NAND_CE1_B = 108,
	MX6UL_PAD_NAND_CLE = 109,
	MX6UL_PAD_NAND_DQS = 110,
	MX6UL_PAD_SD1_CMD = 111,
	MX6UL_PAD_SD1_CLK = 112,
	MX6UL_PAD_SD1_DATA0 = 113,
	MX6UL_PAD_SD1_DATA1 = 114,
	MX6UL_PAD_SD1_DATA2 = 115,
	MX6UL_PAD_SD1_DATA3 = 116,
	MX6UL_PAD_CSI_MCLK = 117,
	MX6UL_PAD_CSI_PIXCLK = 118,
	MX6UL_PAD_CSI_VSYNC = 119,
	MX6UL_PAD_CSI_HSYNC = 120,
	MX6UL_PAD_CSI_DATA00 = 121,
	MX6UL_PAD_CSI_DATA01 = 122,
	MX6UL_PAD_CSI_DATA02 = 123,
	MX6UL_PAD_CSI_DATA03 = 124,
	MX6UL_PAD_CSI_DATA04 = 125,
	MX6UL_PAD_CSI_DATA05 = 126,
	MX6UL_PAD_CSI_DATA06 = 127,
	MX6UL_PAD_CSI_DATA07 = 128,
};

/* Pad names for the pinmux subsystem 
 * 定义了imx6ul平台的pin脚描述符表
 * */
static const struct pinctrl_pin_desc imx6ul_pinctrl_pads[] = {
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE0),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE1),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE2),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE3),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE4),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE5),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE6),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE7),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE8),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE9),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE10),
	IMX_PINCTRL_PIN(MX6UL_PAD_SNVS_TAMPER4),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE12),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE13),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE14),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE15),
	IMX_PINCTRL_PIN(MX6UL_PAD_RESERVE16),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_MOD),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_TMS),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_TDO),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_TDI),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_TCK),
	IMX_PINCTRL_PIN(MX6UL_PAD_JTAG_TRST_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO00),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO01),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO02),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO03),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO04),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO05),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO06),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO07),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO08),
	IMX_PINCTRL_PIN(MX6UL_PAD_GPIO1_IO09),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART1_TX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART1_RX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART1_CTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART1_RTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART2_TX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART2_RX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART2_CTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART2_RTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART3_TX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART3_RX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART3_CTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART3_RTS_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART4_TX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART4_RX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART5_TX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_UART5_RX_DATA),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_RX_DATA0),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_RX_DATA1),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_RX_EN),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_TX_DATA0),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_TX_DATA1),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_TX_EN),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_TX_CLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET1_RX_ER),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_RX_DATA0),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_RX_DATA1),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_RX_EN),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_TX_DATA0),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_TX_DATA1),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_TX_EN),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_TX_CLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_ENET2_RX_ER),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_CLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_ENABLE),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_HSYNC),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_VSYNC),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_RESET),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA00),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA01),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA02),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA03),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA04),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA05),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA06),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA07),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA08),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA09),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA10),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA11),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA12),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA13),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA14),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA15),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA16),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA17),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA18),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA19),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA20),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA21),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA22),
	IMX_PINCTRL_PIN(MX6UL_PAD_LCD_DATA23),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_RE_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_WE_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA00),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA01),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA02),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA03),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA04),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA05),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA06),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DATA07),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_ALE),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_WP_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_READY_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_CE0_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_CE1_B),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_CLE),
	IMX_PINCTRL_PIN(MX6UL_PAD_NAND_DQS),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_CMD),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_CLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_DATA0),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_DATA1),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_DATA2),
	IMX_PINCTRL_PIN(MX6UL_PAD_SD1_DATA3),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_MCLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_PIXCLK),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_VSYNC),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_HSYNC),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA00),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA01),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA02),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA03),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA04),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA05),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA06),
	IMX_PINCTRL_PIN(MX6UL_PAD_CSI_DATA07),
};

// 定义了imx6ul平台pin控制器信息集合
static struct imx_pinctrl_soc_info imx6ul_pinctrl_info = {
	.pins = imx6ul_pinctrl_pads,
	.npins = ARRAY_SIZE(imx6ul_pinctrl_pads),
};

// imx6ul平台pin控制器驱动支持的device描述列表(显然这里只支持1种device描述)
static struct of_device_id imx6ul_pinctrl_of_match[] = {
	{ .compatible = "fsl,imx6ul-iomuxc", },
	{ /* sentinel */ }
};

// imx6ul平台隶属于platform总线的pin控制器驱动和设备匹配成功后的回调函数
static int imx6ul_pinctrl_probe(struct platform_device *pdev)
{
	return imx_pinctrl_probe(pdev, &imx6ul_pinctrl_info);
}

// imx6ul平台隶属于platform总线的pin控制器驱动描述符
static struct platform_driver imx6ul_pinctrl_driver = {
	.driver = {
		.name = "imx6ul-pinctrl",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(imx6ul_pinctrl_of_match),
	},
	.probe = imx6ul_pinctrl_probe,
	.remove = imx_pinctrl_remove,
};

// imx6ul平台隶属于platform总线的pin控制器驱动注册接口
static int __init imx6ul_pinctrl_init(void)
{
	return platform_driver_register(&imx6ul_pinctrl_driver);
}
// pin控制器驱动的启动优先级必须高，因为会有其他驱动依赖该子系统
arch_initcall(imx6ul_pinctrl_init);

static void __exit imx6ul_pinctrl_exit(void)
{
	platform_driver_unregister(&imx6ul_pinctrl_driver);
}
module_exit(imx6ul_pinctrl_exit);

MODULE_AUTHOR("Anson Huang <Anson.Huang@freescale.com>");
MODULE_DESCRIPTION("Freescale imx6ul pinctrl driver");
MODULE_LICENSE("GPL v2");
