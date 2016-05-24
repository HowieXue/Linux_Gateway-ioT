#include "pthread_sqlite.h"	
#include "sqlite3.h"

#define TEMPERATURE_MAX   1
#define TEMPERATURE_MIN   2
#define HUMIDITY_MAX      3
#define HUMIDITY_MIN      4
#define ILLUMINATION_MAX  5
#define ILLUMINATION_MIN  6
#define TELEPHONE_NUM     7
#define MODE              8
#define IFNOCICE          9
#define INTERVAL          10 
#define UPDATE_PERIOD     11
#define USER_NAME         12
#define PASSWORD          13

float callback_val[15];
char callback_str[20];

extern int storageNum;
extern int goodsKinds;

extern struct storage_info storage_no_s;
extern struct env_info_clien_addr env_info_clien_addr_s;
extern struct env_info_clien_addr all_info_RT;
extern struct sqlite_operation sqlite_operation_s;


extern slinklist slinkHead, slinkTail;

/*
**回到函数，查询之后的信息都通过
*/
static int callback_getenv (void * para, int n_column, char ** column_value, char ** column_name )
{
	int i;
#if DEBUG_SQLITE
	printf ("n_column = %d\n", n_column);
#endif
	for (i=0; i<n_column; i++)
	{
		if (n_column-1 == i)
		{
			strcpy (callback_str, column_value[i]);
#if DEBUG_SQLITE
			printf ("%s = %s\n", column_name[i], callback_str);
#endif
		}
		else
		{
			callback_val[i] = (float)atof(column_value[i]);
#if DEBUG_SQLITE
			printf ("%s = %f\n", column_name[i], callback_val[i]);
#endif
		}
	}
#if DEBUG_SQLITE
	printf ("**********************\n");
#endif
	return 0;
}

static int callback_getenv_s (void * para, int n_column, char ** column_value, char ** column_name )
{
	strcpy (callback_str, column_value[0]);
#if DEBUG_SQLITE
	printf ("%s = %s\n", column_name[0], callback_str);
#endif
	return 0;
}

/*
**创建数据库表
*/
int Create_table (void)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = 0;
	int recode;


	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		sprintf (sql,"create table env(dev_no int not null,\
									   temperatureMax float not null,\
									   temperatureMin float not null,\
									   humidityMax float not null,\
									   humidityMin float not null,\
									   illuminationMax float not null,\
									   illuminationMin float not null,\
									   telephoneNum varchar(11) not null,\
				 					   mode int not null,\
									   ifnocice int not null,\
									   interval int not null,\
									   updatePeriod int not null,\
									   username varchar(20) not null,\
									   password int not null);");
		recode = sqlite3_exec (db,sql,0,0,&err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s",err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("creat env OK!\n");
#endif
		}

		sprintf (sql,"create table collect_env(dev_no32 int not null,\
											   temperatureVal float not null,\
											   humidityVal float not null,\
											   illuminationVal float not null,\
											   envtime varchar(10) not null);");
		recode = sqlite3_exec (db,sql,0,0,&err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s",err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("creat collect_env OK!\n");
#endif
			Init_table_env ();
#if DEBUG_SQLITE
			printf ("collect_env init OK!\n");
#endif
		}

		sprintf (sql,"create table goods(dev_no int not null,\
										 goodsId int not null,\
										 goodsCount int not null,\
										 goodsTime varchar(10) not null);");
		recode =sqlite3_exec (db,sql,0,0,&err_msg);
		if (recode != SQLITE_OK)
		{
			printf("Error:%s",err_msg);
			sqlite3_close(db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("creat goods OK!\n");
#endif
			sqlite3_close(db);
			return 0;
		}
	}
}

/*
**初始化环境表，
*/
int Init_table_env (void)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	int i;

	for (i=1; i<=2; i++)
	{
		sprintf (sql,"insert into env values('%d',25.4,10.5,55.8,10.8,\
											 258,129,'12345678901',1,\
											 2,3,4,'binea',123456);", i);
		recode = sqlite3_open (SQLITE_OPEN, &db);
		if (recode != SQLITE_OK)
		{
			printf ("Can't Open Database:%s!\n",sqlite3_errmsg(db));
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("open OK!\n");
#endif
			recode = sqlite3_exec (db,sql,0,0,&err_msg);
			if (recode != SQLITE_OK)
			{
				printf ("Error:%s",err_msg);
				sqlite3_close (db);
				return 1;
			}
			else
			{
#if DEBUG_SQLITE
				printf ("insert env OK!\n");
#endif
				sqlite3_close (db);
			}
		}
	}
	return 0;
}

/*
**更新env参数的SQLite指令选择函数
**这个函数中只能插入int类型的值
*/
int setEnv (float val, int dev_no_t, int no)
{
	char sql[1024];
	switch (no)
	{
	case 1:
		sprintf (sql, "update env set temperatureMax = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
	case 2:
		sprintf (sql, "update env set temperatureMin = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
	case 3:
		sprintf (sql, "update env set humidityMax = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
	case 4:
		sprintf (sql, "update env set humidityMin = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
	case 5:
		sprintf (sql, "update env set illuminationMax = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
	case 6:
		sprintf (sql, "update env set illuminationMin = '%f' where dev_no = '%d';", val, dev_no_t);
		break;
#if 0
	case 7:
		sprintf (sql, "update env set telephoneNum = '%f';", val);
		break;
	case 8:
		sprintf (sql, "update env set mode = '%d';", (int)val);
		break;
	case 9:
		sprintf (sql, "update env set ifnocice = '%d';", (int)val);
		break;
	case 10:
		sprintf (sql, "update env set interval = '%d';", (int)val);
		break;
	case 11:
		sprintf (sql, "update env set updatePeriod = '%d';", (int)val);
		break;
//	case 12:
//		sprintf (sql, "update env set username = '%f';", val);
		break;
	case 13:
		sprintf (sql, "update env set password = '%d';", (int)val);
		break;
#endif
	}

	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode=sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Insert OK!\n");
#endif
			sqlite3_close (db);
			return 0;
		}
	}
}

/*
**更新env参数的SQLite指令选择函数
**这个函数中只能插入字符串类型的值
*/
int setEnv_s (char *val, int no)
{
	char sql[1024];
	switch(no)
	{
	case 7:
		sprintf (sql, "update env set telephoneNum = '%s';", val);
	case 12:
		sprintf (sql, "update env set username = '%s';", val);
	}

	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Insert OK!\n");
#endif
			sqlite3_close (db);
			return 0;
		}
	}
}

/*
**获得env参数的SQLite指令选择函数
*/

int getEnv_select (int storageNum_t, int no)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = 0;
	int recode;

	switch (no)
	{
	case 1:
		sprintf (sql, "select temperatureMax from env where dev_no = '%d';", storageNum_t);
		break;
	case 2:
		sprintf (sql, "select temperatureMin from env where dev_no = '%d';", storageNum_t);
		break;
	case 3:
		sprintf (sql, "select humidityMax from env where dev_no = '%d';", storageNum_t);
		break;
	case 4:
		sprintf (sql, "select humidityMin from env where dev_no = '%d';", storageNum_t);
		break;
	case 5:
		sprintf (sql, "select illuminationMax from env where dev_no = '%d';", storageNum_t);
		break;
	case 6:
		sprintf (sql, "select illuminationMin from env where dev_no = '%d';", storageNum_t);
		break;
#if 0
	case 7:
		sprintf (sql, "select telephoneNum from env;");
		break;
	case 8:
		sprintf (sql, "select mode from env;");
		break;
	case 9:
		sprintf (sql, "select ifnocice from env;");
		break;
	case 10:
		sprintf (sql, "select interval from env;");
		break;
	case 11:
		sprintf (sql, "select updatePeriod from env;");
		break;
	case 12:
		sprintf (sql, "select username from env;");
		break;
	case 13:
		sprintf (sql, "select password from env;");
		break;
#endif
	}
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		if ((7 == no) || (12 == no))
		{
			recode = sqlite3_exec (db, sql, callback_getenv_s, 0, &err_msg);
		}
		else
		{
			recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		}
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("get env OK!\n");
#endif
			sqlite3_close (db);
			return 0;
		}
	}
}

/*
**更新env参数
*/
int updateEnv (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t)
{
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].temperatureMAX, storageNum_t, TEMPERATURE_MAX);
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].temperatureMIN, storageNum_t, TEMPERATURE_MIN);
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].humidityMAX, storageNum_t, HUMIDITY_MAX);
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].humidityMIN, storageNum_t, HUMIDITY_MIN);
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].illuminationMAX, storageNum_t, ILLUMINATION_MAX);
	setEnv (env_info_clien_addr_t.storage_no[storageNum_t].illuminationMIN, storageNum_t, ILLUMINATION_MIN);
//	setEnv_s (storage_no_t->telephoneNum, TELEPHONE_NUM);
//	setEnv (storage_no_t->mode, MODE);
//	setEnv (storage_no_t->ifnocice, IFNOCICE);
//	setEnv (storage_no_t->interval, INTERVAL);
//	setEnv (storage_no_t->updatePeriod, UPDATE_PERIOD);
//	setEnv_s (storage_no_t->username, USER_NAME);
//	setEnv (storage_no_t->password, PASSWORD);

	return 0;
}

/*
**获得env参数
*/
int getEnv (struct env_info_clien_addr *env_info_clien_addr_t, int storageNum_t)
{
	getEnv_select (storageNum_t, TEMPERATURE_MAX);
	env_info_clien_addr_t->storage_no[storageNum_t].temperatureMAX = callback_val[0];
	all_info_RT.storage_no[storageNum_t].temperatureMAX = callback_val[0];	
	getEnv_select (storageNum_t, TEMPERATURE_MIN);
	env_info_clien_addr_t->storage_no[storageNum_t].temperatureMIN = callback_val[0];
	all_info_RT.storage_no[storageNum_t].temperatureMIN = callback_val[0];
	getEnv_select (storageNum_t, HUMIDITY_MAX);
	env_info_clien_addr_t->storage_no[storageNum_t].humidityMAX = callback_val[0];
	all_info_RT.storage_no[storageNum_t].humidityMAX = callback_val[0];
	getEnv_select (storageNum_t, HUMIDITY_MIN);
	env_info_clien_addr_t->storage_no[storageNum_t].humidityMIN = callback_val[0];
	all_info_RT.storage_no[storageNum_t].humidityMIN = callback_val[0];
	getEnv_select (storageNum_t, ILLUMINATION_MAX);
	env_info_clien_addr_t->storage_no[storageNum_t].illuminationMAX = callback_val[0];
	all_info_RT.storage_no[storageNum_t].illuminationMAX = callback_val[0];
	getEnv_select (storageNum_t, ILLUMINATION_MIN);
	env_info_clien_addr_t->storage_no[storageNum_t].illuminationMIN = callback_val[0];
	all_info_RT.storage_no[storageNum_t].illuminationMIN = callback_val[0];
#if 0
	getEnv_select (TELEPHONE_NUM);
	strcpy(storage_info_t->telephoneNum, callback_str);
	getEnv_select (MODE);
	storage_info_t->mode = (int)callback_val[0];
	getEnv_select (IFNOCICE);
	storage_info_t->ifnocice = (int)callback_val[0];
	getEnv_select (INTERVAL);
	storage_info_t->interval = (int)callback_val[0];
	getEnv_select (UPDATE_PERIOD);
	storage_info_t->updatePeriod = (int)callback_val[0];
	getEnv_select (USER_NAME);
	strcpy(storage_info_t->username, callback_str);
	getEnv_select (PASSWORD);
	storage_info_t->password = (int)callback_val[0];
#endif
	return 0;
}

int insertCollect_env (struct env_info_clien_addr env_info_clien_addr_t, int storage_num)
{
#if DEBUG_SQLITE
	printf ("insertCollect_env ok\n");
#endif
#if 1
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	char sql[1024];
	sprintf (sql, "insert into collect_env values(%d,%f,%f,%f,'%s');",
			 storage_num,
			 env_info_clien_addr_t.storage_no[storage_num].temperature,
			 env_info_clien_addr_t.storage_no[storage_num].humidity,
			 env_info_clien_addr_t.storage_no[storage_num].illumination,
			 env_info_clien_addr_t.storage_no[storage_num].samplingTime);
	recode = sqlite3_open(SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Insert collect_env OK!\n");
#endif
			sqlite3_close (db);
			return 0;
		}
	}
#endif
}

int getCollect_env (char itime_t[], int storage_num_t)
{
#if DEBUG_SQLITE
	printf ("getCollect_env ok\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from collect_env where envtime = '%s' and dev_no = %d;", itime_t, storage_num_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("select collect_env OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storage_num_t].temperature = callback_val[1];
		env_info_clien_addr_s.storage_no[storage_num_t].humidity = callback_val[2];
		env_info_clien_addr_s.storage_no[storage_num_t].illumination = callback_val[3];
		strcpy(env_info_clien_addr_s.storage_no[storage_num_t].samplingTime, callback_str);

		all_info_RT.storage_no[storage_num_t].temperature = callback_val[1];
		all_info_RT.storage_no[storage_num_t].humidity = callback_val[2];
		all_info_RT.storage_no[storage_num_t].illumination = callback_val[3];
		strcpy(all_info_RT.storage_no[storage_num_t].samplingTime, callback_str);
#if 1
		printf ("dev_no:%d  temperature = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].temperature);
		printf ("dev_no:%d  humidity = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].humidity);
		printf ("dev_no:%d  illumination = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].illumination);
		printf ("dev_no:%d  time = %s\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].samplingTime);
		printf ("-----------------------------\n");
#endif
	}

	return 0;
}

int getCollect_Current_env (int storage_num_t)
{
#if DEBUG_SQLITE
	printf ("getCollect_env ok\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from collect_env where dev_no = %d order by envtime desc limit 1", storage_num_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("select collect_env OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storage_num_t].temperature = callback_val[1];
		env_info_clien_addr_s.storage_no[storage_num_t].humidity = callback_val[2];
		env_info_clien_addr_s.storage_no[storage_num_t].illumination = callback_val[3];
		strcpy(env_info_clien_addr_s.storage_no[storage_num_t].samplingTime, callback_str);

		all_info_RT.storage_no[storage_num_t].temperature = callback_val[1];
		all_info_RT.storage_no[storage_num_t].humidity = callback_val[2];
		all_info_RT.storage_no[storage_num_t].illumination = callback_val[3];
		strcpy(all_info_RT.storage_no[storage_num_t].samplingTime, callback_str);
		
#if 1
		printf ("dev_no:%d  temperature = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].temperature);
		printf ("dev_no:%d  humidity = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].humidity);
		printf ("dev_no:%d  illumination = %0.1f\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].illumination);
		printf ("dev_no:%d  time = %s\n", storage_num_t, env_info_clien_addr_s.storage_no[storage_num_t].samplingTime);
		printf ("-----------------------------\n");
#endif
	}

	return 0;
}

int insertGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("insertGoods ok\n");
	printf ("storageNum = %d, goodsKinds = %d\n",storageNum_t,goodsKinds_t);
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	char sql[1024];
	sprintf (sql, "insert into goods values(%d,%d,%d,'%s');",
				  storageNum_t,
				  env_info_clien_addr_t.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type,
				  env_info_clien_addr_t.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count,
			      env_info_clien_addr_t.storage_no[storageNum_t].samplingTime);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Insert goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}

	return 0;
}

int deleteGoods (int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("deleteGoods OK\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "delete from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("delete goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}
	return 0;
}

int getGoods (int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("getGoods ok\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];

		all_info_RT.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		all_info_RT.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];
#if DEBUG_SQLITE 
		printf ("goods_s.goods_type = %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type);
		printf ("goods_s.goods_count = %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count);
		printf ("---------------------------------\n");
#endif
	}

	return 0;
}

int getAllGoods (void)
{
#if DEBUG_SQLITE
	printf ("getAllGoods ok\n");
#endif
	return 0;
}

int viewGoods (int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("viewGoods ok\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("view goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}

	return (int)callback_val[2];
}

int addGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("addGoods ok\n");
	printf ("storageNum = %d, goodsKinds = %d\n",storageNum_t,goodsKinds_t);
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode, addNum = 0;
	char sql[1024];
	addNum = env_info_clien_addr_t.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count;

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1; 
		}
		else
		{
#if DEBUG_SQLITE
			printf ("add select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];
		printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%d, %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type, env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count);
	}

	sprintf (sql, "update goods set goodsCount = '%d', goodsTime = '%s' where dev_no = '%d' and goodsId = '%d';",
				  env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count + addNum,
			      env_info_clien_addr_s.storage_no[storageNum_t].samplingTime,
				  storageNum_t,
				  env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("add goods OK!\n");
#endif
		printf ("~~~~~~~~~~~~~~%d, %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type, env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count + addNum);
			sqlite3_close (db);
		}
	}

	return 0;
}

int reduceGoods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("reduceGoods ok\n");
	printf ("storageNum = %d, goodsKinds = %d\n",storageNum_t,goodsKinds_t);
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode, addNum = 0;
	char sql[1024];
	addNum = env_info_clien_addr_t.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count,

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Reduce select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];
	}

	sprintf (sql, "update goods set goodsCount = '%d', goodsTime = '%s' where dev_no = '%d' and goodsId = '%d';",
				  (env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count - addNum),
			      env_info_clien_addr_s.storage_no[storageNum_t].samplingTime,
				  storageNum_t,
				  env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("reduce goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}
	if (0 == viewGoods(storageNum_t, goodsKinds_t))
	{
		deleteGoods (storageNum, goodsKinds);
	}

	return 0;
}


/*
**数据库模块任务
*/
void sqlite_task (struct env_info_clien_addr env_info_clien_addr_t, struct sqlite_operation sqlite_operation_t, int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("sqlite_task!!!\n");  //测试部分，可删。
#endif

	int storage_num;
	if (0 == sqlite_operation_t.table_select_mask)  //0表示对environment表进行操作
	{
		if (0 == sqlite_operation_t.env_operation_mask)  //0表示重新设置环境参数
		{
			updateEnv (env_info_clien_addr_t, storageNum_t);
		}
		else if (1 == sqlite_operation_t.env_operation_mask)  //1表示获得环境参数
		{
			getEnv (&env_info_clien_addr_t, storageNum_t);
		}
	}
	else if (1 == sqlite_operation_t.table_select_mask)  //1表示同时对temperature、humidity和illumination三个表进行操作
	{
		if(0 == sqlite_operation_t.table_operation_mask)  //0表示同时对temperature、humidity和illumination三个表进行插入数据的操作
		{
			for (storage_num=0;storage_num<STORAGE_NUM;storage_num++)
			{
				insertCollect_env (env_info_clien_addr_t, storage_num);
			}
#if 0
			for (storage_num=0;storage_num<STORAGE_NUM;storage_num++)
			{
				getCollect_env (env_info_clien_addr_s.storage_no[storage_num].samplingTime, storage_num);
			}
#endif
		}
		else if (1 == sqlite_operation_t.table_operation_mask)  //1表示同时获得temperature、humidity和illumination三个表数据的操作
		{
			for (storage_num=0;storage_num<STORAGE_NUM;storage_num++)
			{
				getCollect_env (env_info_clien_addr_t.storage_no[storage_num].samplingTime, storage_num);
			}
		}
		else if (2 == sqlite_operation_t.table_operation_mask)  //1表示同时获得temperature、humidity和illumination三个表数据的操作
		{
			for (storage_num=0;storage_num<STORAGE_NUM;storage_num++)
			{
				getCollect_Current_env (storage_num);
			}
		}
	}
	else if (2 == sqlite_operation_t.table_select_mask)  //2表示对goods表进行操作
	{
		if (0 == sqlite_operation_t.goods_operation_mask)  //0表示有新的货物入库
		{
			if (0 == viewGoods(storageNum_t, goodsKinds_t)) //入库货物种类仓库中没有
			{
				insertGoods (env_info_clien_addr_t, storageNum_t, goodsKinds_t);
			}
			else
			{
				addGoods (env_info_clien_addr_t, storageNum_t, goodsKinds_t);
			}
#if 0
			getGoods (storageNum, goodsKinds);
			deleteGoods (storageNum, goodsKinds);
			getGoods (storageNum, goodsKinds);
#endif
		}
		else if (1 == sqlite_operation_t.goods_operation_mask)  //1表示有货物出库
		{
			reduceGoods (env_info_clien_addr_t, storageNum_t, goodsKinds_t);
//			deleteGoods (storageNum, goodsKinds);
		}
		else if (2 == sqlite_operation_t.goods_operation_mask)  //2表示获得指定goodsId的货物信息
		{
			getGoods (storageNum_t, goodsKinds_t);
		}
		else if (3 == sqlite_operation_t.goods_operation_mask)  //3表示获得所有货物信息
		{
			getAllGoods ();
		}
	}
}

void *pthread_sqlite (void *arg)
{
	slinkHead = sqlite_CreateEmptyLinklist();
	slinklist buf = NULL;
#if DEBUG_SQLITE
	printf ("slinkHead ok\n");
#endif
//#if DEBUG_SQLITE
	printf ("pthread_sqlite is ok\n");
//#endif
	while (1)
	{
		pthread_mutex_lock (&mutex_sqlite);
		pthread_cond_wait (&cond_sqlite, &mutex_sqlite);
		pthread_mutex_unlock (&mutex_sqlite);
#if DEBUG_SQLITE
		printf ("pthread_sqlite is up\n");
#endif
#if 0
		while(1 != sqlite_EmptyLinklist(slinkHead))
		{
			sqlite_task (slinkHead->next->data_link, slinkHead->next->data, slinkHead->storageNum, slinkHead->goodsKinds);
			slinkHead = slinkHead->next;
		}
#endif
		while (1)
		{
			pthread_mutex_lock (&mutex_slinklist);
			if ((buf = sqlite_GetLinknode (slinkHead)) == NULL)
			{
				pthread_mutex_unlock (&mutex_slinklist);
				break;
			}
			pthread_mutex_unlock (&mutex_slinklist);
			sqlite_task (buf->data_link, buf->data, buf->storageNum, buf->goodsKinds);
			free (buf);
			buf = NULL;
		}
#if DEBUG_SQLITE
		printf ("info come on\n");
#endif
	}
}




