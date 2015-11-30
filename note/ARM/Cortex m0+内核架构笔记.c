/*******************************************************************************
/*******************************************************************************
*	
*		Cortex-M0+专用外设总线（PPB）地址分布：
*			0xE000E010 - 0xE000E01F		SysTick		(可选的)
*			0xE000E100 - 0xE000E4EF		NVIC
*			0xE000ED00 - 0xE000ED3F		SCB
*			0xE000ED90 - 0xE000EDB8		MPU			(可选的)
*
********************************************************************************/	

	SysTick：系统滴答模块
		地址			寄存器		类型	
		0xE000E010		CTRL		RW			//模块控制和状态寄存器
		0xE000E014		LOAD		RW			//模块重载值寄存器
		0xE000E018		VAL			RW			//模块当前值寄存器
		0xE000E01C		CALIB		RO			//模块校正值寄存器（通常不使用）
		
	-------------------------------------------------------------------
	CTRL:包含定时标志位，时钟源选择位，中断开关位，定时器开关位
	LOAD:写入一个24位长度的重载值
	VAL:存储着一个24位长度的当前值，任何写入操作都会清0该域，同时清0定时标志位
	-------------------------------------------------------------------
	SysTick使用方法：
					1.初始化模块
					2.写入重载值
					3.清除当前值
					4.使能模块

/********************************************************************************/

	NVIC:可嵌套中断向量控制器模块
		地址						寄存器		类型
		0xE000E100					ISER		RW		//外围模块中断使能寄存器	32位对应32个外部中断源IRQ
		0xE000E180					ICER		RW		//外围模块中断禁止寄存器	32位对应32个外部中断源IRQ
		0xE000E200					ISPR		RW		//中断挂起（进入等待）寄存器	写1挂起，写0无反应
		0xE000E280					ICPR		RW		//中断取消挂起寄存器			写1取消挂起，写0无反应
		0xE000E400 - 0xE000E4EF		IPR 0~7		RW		//中断优先级寄存器
		
		----------------------------------------------------------------
		IPRn(0~7):
			这7个寄存器结构相同:	31 30 ... 23 22 ... 15 14 ... 7 6 ...
									每个寄存器分为4个位域，每个位域（8位）只有高2位有效
			这7个寄存器根据IRQ(0~31)将外部中断优先级分成8级，每一级4个，这4个的优先级可以自行调整
		----------------------------------------------------------------
		NVIC使用方法：
					CMSIS库中已经自带了相关函数，通过调用即可

/********************************************************************************/

	SCB：系统控制模块		
		地址			寄存器		类型	
		0xE000ED00		CPUID		RO			//CUP版本号、架构号相关
		0xE000ED04		ICSR		RW			//内部中断控制和状态寄存器
		0xE000ED08		VTOR		RW			//向量表偏移寄存器
		0xE000ED0C		AIRCR		RW			//应用中断和复位控制寄存器
		0xE000ED10		SCR			RW			//系统控制寄存器
		0xE000ED14		CCR			RO			//配置和控制寄存器
		0xE000ED1C		SHPR2		RW			//SVCall的优先级控制寄存器
		0xE000ED20		SHPR3		RW			//SysTick、PendSV优先级控制寄存器
		
		------------------------------------------------------------------
		ICSR:
			[31]:	NMI设置挂起位，写1挂起	
			[28]:	PendSV设置挂起位
			[27]:	PendSV清除挂起位
			[26]:	SysTick设置挂起位
			[25]:	SysTick清除挂起位
			[17:12]:只读，存储中断向量号（非IRQ）
		VTOR:（Cortex M0+手册似乎有误）
			向量表基址默认位于0x00000000,此处可以设置偏移量，从而对向量表在存储空间的地址进行重定向
			1 张向量表尺寸为：64×4=256字节，所以偏移地址必须按256字节对齐
			即[ 31:8 ]位有效，因此向量表重定位范围可以取0x00000000、0x00000100、0x00000200 .... 0xFFFFFF00,
		AIRCR:
			[31:16]:写该寄存器，第一步是写0x05FA到该域，否则任何写操作无效
			[15]:	控制数据存储格式为大/小端
			[2]:	写1产生一个系统级复位
			[1]:	用于DEBUG
		SCR:
			[4]:	用于唤醒处理器
			[2]:	低功耗模式下，用于控制处理器处于浅睡眠还是深度睡眠
			[1]:	还是跟睡眠相关
		CCR:
			[9]:	栈对齐相关，通常读取为1，表示8字节的栈对齐方式
			[3]:	对齐相关，通常读取位1，表示一旦有未对齐出现，就会产生一个HardFault

/********************************************************************************/
	MPU:内存保护单元

		
/*******************************************************************************		
/*******************************************************************************		
/*******************************************************************************
*	
*		Cortex-M0+内核寄存器：
*			通用寄存器：	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12
*			栈指针：		SP(R13)		->	PSP	/ MSP
*			链接寄存器：	LR(R14)
*			程序计数器：	PC(R15)
*			程序状态寄存器：PSR
*			中断屏蔽寄存器：PRIMASK
*			控制寄存器：	CONTROL
********************************************************************************/

	SP:		在thread mode 下，CONTROL中的[1]位决定了SP是MSP还是PSP
			在handle mode 下，SP只能是MSP
			MSP的值来自地址0x00000000上
	LR:		当进行函数调用、执行异常等时，该寄存器保存了返回地址
	PC:		保存的是当前程序运行地址，reset时，自动载入0x00000004地址上的值
	PSR:	31 			30 			29 			28 		...		24 		... 	5 4 3 2 1 0
			N  			Z  			C  			V   	    	T      			中断异常号	
		 负标志      零标志  进位/借位标志   溢出标志		Thumb标志			（0～47）	
			PSR其实是由3个寄存器（APSR、EPSR、IPSR）复合而成
	CONTROL:包含MSP/PSP选择位、thread mode下处理器特权选择位
	注意点：操作这些内核寄存器必须通过 MSR/MRS指令 + 各自寄存器名	来完成。
	

/*******************************************************************************		
/*******************************************************************************		
/*******************************************************************************
*	
*		ARM汇编部分
*
********************************************************************************/		
汇编中几个常用的段代号，基本跟编译器和处理器都没有关系：
	.text		//代码段，具有可执行的属性
	.data		//读写数据段，存放已初始化全局变量
	.const		//只读数据段（有些编译器不使用此段，将只读数据并入.data段）
	.bss		//未初始化的全局变量数据段，

//-----------------------------------------------------------------------------
			