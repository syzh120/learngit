/***********************************************************************************************************************
**
*												任务管理部分		
**
************************************************************************************************************************/

1. 任务控制块（TCB）:用于保存一个任务的状态，双链表结构
			typedef struct os_tcb
			{
				OS_STK		*OSTCBStkPtr;
				
				#if OS_TASK_CREATE_EXT_EN >0
					void 	*OSTCBExtPtr; 
					OS_STK	*OSTCBStkBottom;
					INT32U	OSTCBStkSize;
					INT16U	OSTCBOpt;
					INT16U	OSTCBId;
				#endif
				
				struct os_tcb *OSTCBNext;
				struct os_tcb *OSTCBPrev;
				
				#if ((OS_Q_EN>0)&&(OS_MAX_QS>0)) || (OS_MBOX_EN>0) || (OS_SEM_EN>0) || (OS_MUTEX_EN>0)
					OS_EVENT *OSTCBEventPtr;
				#endif
				
				#if ((OS_Q_EN>0)&&(OS_MAX_QS>0)) || (OS_MBOX_EN>0)
					void 	*OSTCBMsg;
				#endif
				
				INT16U	OSTCBDly;
				INT8U	OSTCBStat;
				INT8U	OSTCBPrio;
				
				INT8U	OSTCBX;
				INT8U	OSTCBY;
				INT8U	OSTCBBitX;
				INT8U	OSTCBBitY;	
			}OS_TCB;
	*OSTCBStkPtr:				指向当前任务堆栈栈顶的指针，放在结构最前面，可以方便汇编时处理该变量
	当OS_TASK_CREATE_EXT_EN设为1时，即允许建立用户自定义的任务函数扩展时，则另有增加些变量：（通常不需要用到）
		*OSTCBExtPtr:			指向用户自定义的任务扩展控制块
		*OSTCBStkBottom:		指向指向任务堆栈栈底指针
		OSTCBStkSize:			保存栈中可容纳的指针元数目，注意不是用字节表示的
		OSTCBOpt:				保存选择项
		OSTCBId：				保存任务识别码
	*OSTCBNext && *OSTCBPrev：	用于任务控制块TCB双向链表的前后链接，通常只用作单链表。OSTimeTick()中使用它来刷新各任务的任务 OSTCBDly 变量
							  	每个任务建立时，其TCB从空链表池中取出，链接到链表中，删除任务时，从链表中删除，放回空链表池
	*OSTCBEventPtr：			指向ECB的指针
	*OSTCBMsg：					指向传递给任务的消息的指针
	OSTCBDly：					当需要把任务延时或挂起一段时间时，这个变量保存任务允许等待的最多时钟节拍数;如果为0，则表示不延时或等待不限时
	OSTCBStat：					保存任务的状态标志，可以是就绪态/等待SEM态/等待MBOX态/等待Q态/等待MUTEX态等
	OSTCBPrio：					保存任务的优先级
	OSTCBX，OSTCBY，OSTCBBitX，OSTCBBitY：用于加速任务进入就绪态或等待事件发生态的过程，建立任务时就算好，计算方法：
										OSTCBY		= prio>>3;			//高5位
										OSTCBX		= prio&0x07			//低3位
										OSTCBBitY	= 1<<OSTCBY			
										OSTCBBitX	= 1<<OSTCBX

2. 创建任务
	INT8U OSTaskCreate (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio)	//task是任务的函数指针，pdata是传递给任务的参数的指针，ptos是分配给任务的堆栈的栈顶指针，prio是分配给任务的优先级
		1. 判断分配给任务的优先级是否有效;确保并未处于中断中;然后确保该优先级并未分配给已经创建的任务（已经创建的任务会将TCB地址存入OSTCBPrioTbl[]数组中）
		2. 如果该优先级确实空闲未用，放一个非空指针在OSTCBPrioTbl[]中来保留该优先级，这样执行后面代码时能重新允许中断
		3. 初始化将要创建的任务的堆栈，其实就是模拟一个异常响应的过程：
			   依次把xPSR、LR、R12、R3、R2、R1、R0压入堆栈（已知栈顶ptos），R7~R4为可选压入，最后返回新的栈顶指针，并保存在TCB中
		4. 接下去就是对TCB进行初始化：
			   从空余的TCB链表池中取出一个TCB，将链表表头指向下一个空余TCB；
			   接着对取出的TCB内成员进行初始化；
			   然后新创建任务的TCB地址存入代表已经创建任务的数组OSTCBPrioTbl[]中；
			   同时将该TCB插入到已经建立任务的双向链表中，表头为OSTCBList，新任务的TCB总是插到表头；
			   最后将任务添加进就绪表。
		5. 任务调度
		
	INT8U OSTaskCreateExt(void   (*task)(void *p_arg),  		/*任务函数指针 */
	                        void    *p_arg,						/*传递参数指针*/		
	                        OS_STK  *ptos,						/*分配任务堆栈栈顶指针 */
	                        INT8U    prio,						/*分配任务优先级 */
	                        INT16U   id,						/*(为将来预留)优先级标识(目前暂时与优先级相同)*/
	                        OS_STK  *pbos,						/*分配任务堆栈栈底指针*/
	                        INT32U   stk_size,  				/*指定堆栈的容量(堆栈检验用)*/
	                        void    *pext,						/*指向用户附加的数据域的指针，用于填充任务TCB，从而扩展TCB模块，选项可以是FPU、MMU、0...*/
	                        INT16U   opt)						/*建立任务设定选项，有三个选项：OS_TASK_OPT_STK_CHK、OS_TASK_OPT_STK_CLR、OS_TASK_OPT_SAVE_FP */		   

3. 堆栈检验
	INT8U  OSTaskStkChk (INT8U prio, OS_STK_DATA *p_stk_data)			//用优先级piro代表要检验的任务，数据结构OS_STK_DATA用来存放该任务已使用堆栈和未使用堆栈数量
	堆栈检验的基本原理：由栈底往上计算空余堆栈空间大小，具体实现方法是统计存储值为0的连续堆栈的数目。
	注意点：数据结构OS_STK_DATA里成员虽然是uint32_t类型，但实际存放进入到堆栈使用/未使用数量是以字节为单位的，所以会造成一种4倍堆栈尺寸的错觉（因为堆栈尺寸单位是OS_STK）。
	

/***********************************************************************************************************************
**
*												事件管理部分		
**
************************************************************************************************************************/
1. 事件控制块（ECB）：用于通信的单链表结构，创建一个事件（邮箱、消息队列、信号量、互斥体）的函数里，其本质就是在初始化这个事件控制块。
			typedef struct
			{
				INT8U	OSEventType;					
				void   *OSEventPtr;
				INT16U	OSEventCnt;
				INT8U	OSEventGrp;
				INT8U	OSEventTbl[OS_EVENT_TBL_SIZE];
			}OS_EVENT;
	OSEventType：事件类型，其值分别对应MBOX邮箱、Q消息队列、SEM信号量、MUTEX互斥体、FLAG事件标志
	*OSEventPtr: 只有事件是互斥体、邮箱或消息队列时才使用，否则初始化为空指针。
				 当事件是互斥体时，指向占用MUTEX的任务的TCB	
				 当事件是消息邮箱时，指向消息的初始值，该初始值通常是NULL的指针 
				 当事件是消息队列时，指向一个数据结构---队列控制块OS_Q			
	OSEventCnt:	 当事件是互斥体时，高8位用于保存继承优先级PIP值，低8位为占用当前MUTEX的任务的优先级（初始化时因无任务占用，为0xff）;
				 当事件是信号量时，用作信号量的计数器，初始值为0～65535
	OSEventGrp && OSEventTbl[]: 2者共同包含了一系列正在等待某事件发生的任务，这些等待中的任务按优先级分为 OS_EVENT_TBL_SIZE(0~7) 组，每组8个。2者的对应关系如下：
								OSEventGrp				OSEventTbl[OS_EVENT_TBL_SIZE]
								[0]		---------		[7] [6] [5] [4] [3] [2] [1] [0]
								[1]		---------		[15][14][13][12][11][10][9] [8]
								[2]		---------		[23][22][21][20][19][18][17][16]
								[3]		---------		[31][30][29][28][27][26][25][24]
								[4]		---------		[39][38][37][36][35][34][33][32]
								[5]		---------		[47][46][45][44][43][42][41][40]
								[6]		---------		[55][54][53][52][51][50][49][48]
								[7]		---------		[63][62][61][60][59][58][57][56]
								OSEventTbl[]数组长度与系统中任务的最低优先级有关，这样做可以节省RAM，上表是完整版的，实际并不一定需要完整
								
	注意点：
		1. 任务或中断服务程序可以给事件控制块ECB发信号;
		2. 只有任务可以等待另一个任务或中断服务程序通过ECB给它发信号，而中断服务程序是不能等待ECB给它发信号的;
		3. 处于等待状态的任务可以指定一个最长等待时间，以防止无限等待;
		4. 多个任务可以同时等待同一事件发生，当该事件发生后，所有等待任务中优先级最高的任务得到该事件并进入就绪状态
	
2. 信号量
	OS_EVENT  *OSSemCreate (INT16U cnt)：建立一个信号量。
		一般做了以下几件事：
			1. 判断是否处于中断中，所有建立事件的工作必须在任务级代码中或者多任务启动前完成，中断服务程序中不能建立任何事件（以下任何事件！！！）;
			2. 从空余事件控制块链表中获得一个ECB，将链表表头指向下一个空余事件控制块
			3. 将获得的ECB成员按信号量初始化，其中的等待任务列表是通过调用系统自带函数完成清0初始化
	
	void  OSSemPend (OS_EVENT *pevent, INT32U timeout, INT8U *perr)：等待一个信号量。
		一般做了以下几件事：
			1. 判断是否处于中断中，中断服务函数不能等待事件;
			2. 如果信号量的计数器非0，则计数值递减，该等待函数返回无错标志
			3. 如果信号量的计数器为0，则调用该等待函数的任务进入睡眠状态，进入前需对任务控制块TCB中成员做相应标记，然后调用系统自带函数将任务置入睡眠（等待）状态
			  （注意点：睡眠的是任务，而不是该等待函数，所以等待函数还是再往下执行）
			4. 然后进行任务调度，让下一个优先级最高的任务运行
			5. 当信号量有效或等待超时时，等待函数继续运行任务调度之后的代码，通过判断TCB中的状态标志来区分是信号量有效还等待超时。
			   如果是超时，则通过调用系统自带函数将任务从等待任务列表删除，该等待函数返回超时标志;
			   如果信号量有效，则清除TCB中相应标志，该等待函数返回无错标志。
	
			
	INT8U  OSSemPost (OS_EVENT *pevent)：释放一个信号量。
		一般做了以下几件事：
			1. 判断等待列表中是否有任务在等待该信号量;
			2. 如果有，则调用系统自带函数把优先级最高的任务从等待任务列表中去除，并使它进入就绪态，然后进行任务调度，让优先级最高的就绪态任务运行
			   如果没有，则信号量的计数器加1，并确保不会溢出
		注意点：当中断服务程序调用该函数，不会发生任务切换,必须等到中断全部退出后
		
3. 互斥体
	互斥体MUTEX是一种二值信号量，只能供任务使用，意味着不能在中断中使用，并且只能用于处理共享资源。
	MUTEX通常用于解决优先级反转问题，方法是：
		把占用MUTEX的低优先级任务的优先级提到略高于等待MUTEX的高优先级任务的优先级，当共享资源使用完时，释放MUTEX，同时将优先级恢复到原来的水平。
	OS_EVENT  *OSMutexCreate (INT8U   prio, INT8U  *perr)：建立一个互斥体
		一般做了以下几件事：
			1. 判断是否处于中断中，所有建立事件的工作必须在任务级代码中或者多任务启动前完成，中断服务程序中不能建立任何事件（以下任何事件！！！）;
			2. 确认PIP没有被任何任务占用，并保留这个优先级，防止该优先级建立别的任务或其他MUTEX使用这个优先级
			3. 从空余事件控制块链表中获得一个ECB，将链表表头指向下一个空余事件控制块
			4. 将获得的ECB成员按互斥体初始化，其中的等待任务列表是通过调用系统自带函数完成清0初始化
	
	void  OSMutexPend (OS_EVENT  *pevent, INT32U     timeout, INT8U     *perr)：等待一个互斥体（挂起）。
		一般做了以下几件事：
			1. 判断是否处于中断中，中断服务函数不能等待事件;
			2. 通过OSEventCnt的低8位判断MUTEX是否有效，如果有效，低8位等于调用该等待函数的任务的优先级，OSEventPtr指向调用该等待函数的任务的TCB,该等待函数返回无错标志;
			3. 如果无效，意味着有任务已经占用了MUTEX,所以提取MUTEX的PIP、占用MUTEX任务的优先级和TCB  
			4. 接着比较占用MUTEX任务的优先级和调用该等待函数的任务的优先级，如果前者低，则需要再做一步提升优先级的操作;
			5. 调用该等待函数的任务的TCB中成员做相应标记后，调用系统自带函数将任务置入睡眠（等待）状态;
			6. 然后进行任务调度，让下一个优先级最高的就绪态任务运行  
			7. 当MUTEX有效或等待超时时，等待函数继续运行任务调度之后的代码，通过判断TCB中的状态标志来区分是信号量有效还等待超时。
			   如果是超时，则通过调用系统自带函数将任务从等待任务列表删除，该等待函数返回超时标志;
			   如果信号量有效，则清除TCB中相应标志，该等待函数返回无错标志。
	
	INT8U  OSMutexPost (OS_EVENT *pevent)：释放一个互斥体。
		一般做了以下几件事：
			1. 判断是否处于中断中，中断服务函数不能释放互斥体; 
			2. 判断占用MUTEX的任务的优先级是否是PIP,如果是，就要把当前优先级恢复到原来的优先级
			3. 判断等待列表中是否有任务在等待该信号量;
			4. 如果有，则调用系统自带函数把优先级最高的任务从等待任务列表中去除，并使它进入就绪态，然后进行任务调度，让优先级最高的就绪态任务运行
			   如果没有，设置OSEventCnt的低8位，表明MUTEX处于有效状态


4. 消息邮箱
	消息邮箱MBOX可以使一个任务或中断服务程序向另一个任务发送一个指针型变量
	OS_EVENT  *OSMboxCreate (void *pmsg)：建立一个邮箱，*pmsg为消息初始值
		一般做了一下几件事：
			1. 判断是否处于中断中，所有建立事件的工作必须在任务级代码中或者多任务启动前完成，中断服务程序中不能建立任何事件（以下任何事件！！！）;	 
			2. 从空余事件控制块链表中获得一个ECB，将链表表头指向下一个空余事件控制块
			3. 将获得的ECB成员按邮箱初始化，其中的等待任务列表是通过调用系统自带函数完成清0初始化 
			
	void  *OSMboxPend (OS_EVENT *pevent,INT32U timeout,INT8U *perr):等待一个邮箱中的消息。
		一般做了以下几件事：
			1. 判断是否处于中断中，中断服务函数不能等待事件;
			2. 如果OSEventPtr是个非NULL指针时，说明该邮箱中有可用的消息，返回无错标志和消息指针;
			3. 否则，意味着邮箱中没有可用的消息，则调用该等待函数的任务进入睡眠状态，进入前需对任务控制块TCB中成员做相应标记，然后调用系统自带函数将任务置入睡眠（等待）状态
			4. 然后进行任务调度，让下一个优先级最高的就绪态任务运行;
			5. 当邮箱中有可用消息或等待超时时，等待函数继续运行任务调度之后的代码，通过判断TCB中的状态标志来区分是有可用消息还是等待超时;
			   如果是超时，则通过调用系统自带函数将任务从等待任务列表删除，清楚TCB中相应标志，该等待函数返回超时标志;
			   如果消息有效，则清除TCB中相应标志，该等待函数返回无错标志和消息指针。
			   
	INT8U  OSMboxPost (OS_EVENT *pevent, void *pmsg)：发送一个消息到邮箱中
		一般做了以下几件事：
			1. 检查是否有任务在等待该邮箱中的消息
			2. 如果有，则调用系统自带函数把优先级最高的任务从等待任务列表中去除，并使它进入就绪态，然后进行任务调度，让优先级最高的就绪态任务运行
			   如果没有，指向消息的指针保存到邮箱中（前提是邮箱中没有可用消息）


5. 消息队列
	消息队列实质上是一种邮箱阵列，可以传送多条消息。
	实现消息队列需要额外2种数据结构：
		1. 队列控制块OS_Q:
		   typedef struct{
		       OS_Q 	*OSQPtr;		//在空闲队列控制块链表中，指向下一个OS_Q;一旦取出建立消息队列，就不再使用
		       void 	**OSQStart;		//指向消息队列的指针数组的起始地址的指针
		       void 	**OSQEnd;		//指向消息队列的指针数组的结束单元的下一个地址的指针
		       void 	**OSQIn;		//指向消息队列中插入下一条消息的位置的指针，当OSQIn=OSQEnd时，OSQIn指向起始单元
		       void 	**OSQOut;		//指向消息队列中取出下一条消息的位置的指针，当OSQOut=OSQEnd时，OSQOut指向起始单元
		       INT16U	OSQSize;		//消息队列中可容纳的总的消息数
		       INT16U	OSQEntries; 	//消息队列中当前消息数
		   	}OS_Q;
		2. void *MsgTbl[OSQSize]:开辟的存放消息的空间

	--------------------------------------------------------------------------------------------------------------
	OS_EVENT  *OSQCreate (void **start,INT16U size)：建立一个消息队列，start为指向消息数组的指针，size为该数组大小
	一般做了以下几件事：
		1. 判断是否处于中断中，所有建立事件的工作必须在任务级代码中或者多任务启动前完成，中断服务程序中不能建立任何事件（以下任何事件！！！）;
		2. 从空余事件控制块链表中获得一个ECB，将链表表头指向下一个空余事件控制块；
		3. 从空余队列控制块链表中获得一个OS_Q,将链表表头指向下一个空余队列控制块；
		4. 对取出的OS_Q内成员进行初始化，对取出的ECB内成员进行初始化，特别是将OSEventPtr指向取出的OS_Q控制块
		5. 对等待任务列表是通过调用系统自带函数完成清0初始化


/***********************************************************************************************************************
**
*												任务调度		
**
************************************************************************************************************************/
任务调度分2种：
			任务级调度：OSSched()	//主要方式
			中断级调度：OSIntExt()	//这里暂时不用，略
void OSSched(void)
{
	INT8U y;
	
	OS_ENTER_CRITICAL();								
	if((OSIntNesting == 0) || (OSLockNesting == 0))				//确保不在中断中，且调度函数未上锁
	{
		y =	OSUnMapTbl[OSRdyGrp];								//通过查OSUnMapTbl表找到就绪表中优先级最高的任务
		OSPrioHighRdy = OSUnMapTbl[OSRdyTbl[y]] + (INT8U)(y<<3);
		
		if(OSPrioHighRdy != OSPrioCur)							//判断就绪表中优先级最高的任务是否就是当前正在运行的任务，避免不必要的调度
		{
			OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];			//从已经被分配的TCB双向链表池中找到该优先级对应的TCB控制块
			OSCtxSwCtr++;										//统计任务切换次数的变量加1，没啥用
			OS_TASK_SW();										//重头戏，真正开始进行上下文切换，涉及硬件，该函数汇编写	
		}
	}
	OS_EXIT_CRITICAL();	
}
-------------------------------------------------------------------------------------------------------------------------
	SCB_ICSR				EQU	0xE000ED04						//ARM内核SCB模块中ICSR寄存器地址
	SCB_ICSR_PENDSVSET		EQU 0x10000000						//ICSR寄存器中PendSV设置挂起位
	SCB_SHPR3				EQU	0xE000ED20						//ARM内核SCB模块中SHPR3寄存器地址
	SCB_SHPR3_PENDSV_PRIO	EQU	0xFF0000						//SHPR3寄存器中PendSV异常的优先级设置，0xFF表示设为最低优先级						
	EXPORT OS_TASK_SW											//声明标签（也就是函数）OS_TASK_SW为外部可用
	EXTERN OSTaskSwHook											//声明标签（也就是函数）OSTaskSwHook为外部定义
	
OS_TASK_SW														//挂起PendSV异常
	LDR R0,=SCB_ICSR											
	LDR R1,=SCB_ICSR_PENDSVSET
	STR	R1,[R0]
	BX  LR
	
OSPendSvHandler													//执行PendSV异常服务程序，即保存旧任务现场（入栈），取出新任务现场（出栈）
	MRS		R3,PRIMASK											//读取内核寄存器PRIMASK值到R3
	CPSID	I													//屏蔽中断
	
	MRS		R0,PSP												//读取当前进程堆栈的指针到R0
	CMP		PSP,#0												//如果是系统第一次任务切换，则跳转到OSPendSV_NoSave
	BEQ		OSPendSV_NoSave
	
	SUBS	R0,R0,#0x10											//否则就需要保存现场，先让堆栈指针SP-0x10（因为栈向下生长、这里就是下移4字，为R4-R7留出空间）
	STM		R0,{R4-R7}											//将R4-R7寄存器值保存到SP指向地址，即入栈
																//注意点这个入栈过程中内核有8个寄存器（PSR\PC\LR\R12\R3\R2\R1\R0）是自动保存的
	LDR		R1,=OSTCBCur										//读取存储当前任务控制块的基地址值的地址！！！切记！！！
	LDR		R1,[R1]												//取出当前任务控制块的基地址
	STR		R0,[R1]												//将当前任务的堆栈指针替换为当前内核SP，从而真正完成了保存旧任务现场

OSPendSV_NoSave	
	PUSH	{R14}												//这几句用于实现用户扩展，可以省略
	LDR		R0,=OSTaskSwHook
	BLX		R0
	POP		{R0}
	MOV		R14,R0
	
	LDR		R0,=OSPrioCur										//将当前优先级替换为就绪状态的最高优先级
	LDR		R1,=OSPrioHighRdy
	LDRB	R2,[R1]
	STRB	R2,[R0]
	
	LDR		R0,=OSTCBCur										//将当前TCB替换为就绪状态的最高优先级任务的TCB
	LDR		R1,=OSTCBHighRdy
	LDRB	R2,[R1]
	STRB	R2,[R0]												//这里的R2中保存的是OSTCBHighRdy的基地址
	
	LDR		R0,[R2]												//将新任务的堆栈指针赋值给R0，其实就是内核SP
	LDM		R0!,{R4-R7}											//SP从这个新任务的堆栈指针处逆向取出R4-R7寄存器的值，同时内核中的8个寄存器值是自动取出的
	
	MSR		PSP,R0												//将R0中的SP赋值给PSP寄存器
	
	ORR     LR, LR, #0x04										//LR寄存器[2]位置1，表示从进程堆栈中做出栈操作，返回后使用PSP
    
    MSR		PRIMASK,R3
    BX		LR
    
 小结：	进入异常服务程序后,LR原本的值已经被自动保存入栈，然后LR的用法被重新解释，其值被自动更新为特殊的EXC_RETURN,这是个高28位全为1，只有[ 3:0 ]有效的值
 		当异常服务程序执行完毕后，通常使用 "BX	LR" 指令，把这个值送往PC时，就会启动异常返回序列
	


/***********************************************************************************************************************
**
*												时间管理		
**
************************************************************************************************************************/
void  OSTimeDly (INT32U ticks)：任务延时函数
	延时的长短由指定的ticks时钟节拍数确定的;
	延时期间,首先是从就绪表中移出被延时的任务（注意，并没有放入等待列表！！），同时将延时节拍数保存到该任务的TCB中；
	然后进行一次任务调度，从而去执行下一个优先级最高的就绪态任务；
	每过一个时钟节拍，OSTimeTick()将延时节拍数减1，一旦延时满或取消延时，被延时的任务会重新放入就绪表。
	但只有当该任务在所有就绪态任务中具有最高优先级时才会立即执行。
	ticks用法：OS_TICKS_PER_SEC*n
			   OS_TICKS_PER_SEC为全局变量，意为1秒的时钟节拍数
INT8U  OSTimeDlyHMSM (INT8U   hours,INT8U   minutes,INT8U   seconds,INT16U  ms)	：可以按时、分、秒、毫秒来定义延时时间，用法同OSTimeDly。