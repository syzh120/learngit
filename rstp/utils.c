
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <linux/un.h>
#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <net/if.h>

#include "utils.h"

#define SYSFS_PATH_MAX 256

int netsock = -1;   // 用于ioctl

// 过滤函数，寻找包含bridge目录的文件夹
static int _isbridge(const struct dirent *entry)
{
	char path[SYSFS_PATH_MAX];
	struct stat st;

	snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/%s/bridge", entry->d_name);
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

// 过滤函数，滤掉"."和".."这两个文件
static int _filter_dot(const struct dirent *entry)
{
	const char *n = entry->d_name;

	return !(n[0] == '.' && (n[1] == 0 || (n[1] == '.' && n[2] == 0)));
}

int set_bridge_stp(char *br_name, int stp_val)
{
	char path[SYSFS_PATH_MAX];
	FILE *f;
	
	snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/%s/bridge", br_name);
	f = fopen(path, "w");
	if (f) {
		fprintf(f, "%ld\n", stp_val);
		fclose(f);
	}

	return 0;
}

int get_hwaddr(char *ifname, unsigned char *hwaddr)
{
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(netsock, SIOCGIFHWADDR, &ifr) < 0) {
		return -1;
	}
	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	
	return 0;
}

// 获取桥名
int get_bridge_name(char *br_name) 
{
	int bridge_count;
	struct dirent **namelist;

    // 在/sys/class/net目录下寻找一个文件夹，内含一个bridge的子文件夹
	bridge_count = scandir("/sys/class/net", &namelist, _isbridge, alphasort);
	if((bridge_count < 0) || (bridge_count > 1)) {
		return -1;
	}

	namelist[0]->d_name[IFNAMSIZ - 1] = '\0';
	strncpy(br_name, namelist[0]->d_name, IFNAMSIZ);
    
    // 这里需要添加释放namelist内存的函数
    free(namelist[0]);
    free(namelist);
	
	return 0;
}

// 获取mac
int get_if_mac(int if_index, unsigned char *mac) 
{
	struct ifreq ifr;
	char if_name[IFNAMSIZ];

	if_indextoname(if_index, if_name);
	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name));
    
    // 调用ioctl获取指定接口名的mac
	if (ioctl(netsock, SIOCGIFHWADDR, &ifr) < 0) {
		return -1;
	}
	memcpy(mac, &ifr.ifr_hwaddr.sa_data[0], 6);
	
	return 0;
}

// 获取端口数量
int get_port_count(char *br_name, int *count) 
{
	int port_count;
	char path[SYSFS_PATH_MAX];
	struct dirent **namelist;
	
	sprintf(path, "/sys/class/net/%s/brif", br_name);
    // 获取/sys/class/net/br_lan/brif 目录下除了"."和".."之外的文件数量
	port_count = scandir(path, &namelist, _filter_dot, alphasort); 
	if (port_count < 0) {
		return -1;
	}
	*count = port_count;
	
    // 这里需要添加释放namelist内存的函数
    free(namelist[0]);
    free(namelist);

	return 0;
}

// 创建ioctl套接字
int netsock_init(void)
{
	netsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (netsock < 0) {
		return -1;
	}
	return 0;
}
