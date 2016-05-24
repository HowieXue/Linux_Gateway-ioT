#ifndef __SQLITE_TEST_H__
#define __SQLITE_TEST_H__

#include "data_global.h"
#include "sqlite_link_list.h"
#include "sqlite3.h"

#define DEBUG_SQLITE 0

extern pthread_cond_t cond_sqlite;
extern pthread_mutex_t mutex_sqlite;
extern pthread_mutex_t mutex_s;
extern pthread_mutex_t mutex_slinklist;
extern pthread_mutex_t mutex_sqlite;
#if 0
extern int table_select_mask;
extern int env_operation_mask;
extern int table_operation_mask;
extern int goods_operation_mask;
#endif
extern struct env env_s;
extern struct collect_env collect_env_s;
extern struct goods goods_s;
extern struct storage_info storage_no_s;
extern struct env_info_clien_addr env_info_clien_addr_s;
extern struct env_info_clien_addr all_info_RT;

//extern int storagenum;
//extern int goodsKinds;



int Create_table (void);  //创建数据库表
int  Init_table_env (void);  //初始化环境参数表

void sqlite_task(struct env_info_clien_addr env_info_clien_addr_t, struct sqlite_operation sqlite_operation_t, int storageNum_t, int goodsKinds_t);

int updateEnv (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t);  //更新整个环境参数
int getEnv (struct env_info_clien_addr *env_info_clien_addr_t, int storageNum_t);  //获得整个环境参数
int insertCollect_env (struct env_info_clien_addr env_info_clien_addr_t, int storage_num);
int getCollect_env (char itime_t[], int storage_num_t);
int getCollect_Current_env (int storage_num_t);
int insertGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);  //插入新的货物信息
int deletGoods (int storageNum_t, int goodsKinds_t);  //删除指定编号货物信息
int getGoods (int storageNum_t, int goods_kinds_t);  //获得指定编号货物信息
int getGoodsInfo (void);  //获得所有货物信息
int viewGoods (int storageNum_t, int goodsKinds_t);  //查看仓库是否存在此类货物
int addGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);
int reduceGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);








#endif
	  
