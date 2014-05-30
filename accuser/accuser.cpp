//============================================================================
// Name        : accuser.cpp
// Author      : yaocoder
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <fstream>

#define LOG_INFO(...) 	std::cout << "\033[01;34m " << __VA_ARGS__ << "\033[0m" << std::endl;
#define LOG_ERROR(...) 	std::cout << "\033[31m "	<< __VA_ARGS__ << "\033[0m" << std::endl;


int village_count = 0;

typedef struct accuserInfo_
{
	std::string accuser_name;
	int	 number;
	std::vector<std::string> vec_accused_name;

	accuserInfo_()
	{
		number = 0;
	}

}ACCUSER_INFO;

std::set<std::string> setA_village;
std::set<std::string> setB_village;

static bool GetAppPath(std::string& app_path);
static void usage();
static bool ReadConf(const std::string& file_fullpath, std::list<ACCUSER_INFO>& list_accuserInfo);
static void LogicOpt(std::list<ACCUSER_INFO>& list_accuserInfo);

int main(int argc, char **argv)
{
	int c;
	std::string filename;
	if (-1 != (c = getopt(argc, argv,
		  "h"		/* usage */
		  "f:"		/* filename */
		  )))
	{
		switch (c)
		{
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		case 'f':
			filename.assign(optarg);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		LOG_ERROR("argv error. please input '-h'.");
		exit(EXIT_FAILURE);
	}

	/* 得到当前程序的路径*/
	std::string app_path;
	if(!GetAppPath(app_path))
	{
		LOG_ERROR("GetAppPath error.");
		exit(EXIT_FAILURE);
	}

	/* 加载配置文件得到ACCUSER_INFO */
	std::string file_fullpath = app_path + filename;
	std::list<ACCUSER_INFO> list_accuserInfo;
	if(!ReadConf(file_fullpath, list_accuserInfo))
	{
		return EXIT_FAILURE;
	}

	/* 逻辑处理 */
	LogicOpt(list_accuserInfo);

	int more = 0, less = 0;
	if(setA_village.size() > setB_village.size())
	{
		more = setA_village.size();
		less = setB_village.size();
	}
	else
	{
		more = setB_village.size();
		less = setA_village.size();
	}

	LOG_INFO(more << " " << less);

	return EXIT_SUCCESS;
}

bool GetAppPath(std::string& app_path)
{
	char buf[1024] = {0};
	int n;

	n = readlink("/proc/self/exe", buf, sizeof(buf));
	if (n > 0 && n < (int)(sizeof(buf)))
	{
		app_path.assign(buf);
		app_path = app_path.substr(0, app_path.find_last_of("/")+1);
		return true;
	}

	return false;
}

void usage()
{
	LOG_INFO("Usage:");
	LOG_INFO("-f  [filename]   --  input filename must be 'my.conf'");
}

bool ReadConf(const std::string& file_fullpath, std::list<ACCUSER_INFO>& vec_accuserInfo)
{
	std::ifstream in(file_fullpath.c_str());
	if (!in.is_open())
	{
		LOG_ERROR("open file error.");
		std::cerr << "error = " << strerror(errno) <<  std::endl;
		return false;
	}

	std::string str;
	if(in >> str)
	{
		village_count = atoi(str.c_str());
	}

	for (int i = 0; i < village_count; ++i)
	{
		ACCUSER_INFO accuserInfo;
		if (in >> str)
		{
			accuserInfo.accuser_name = str;
		}

		if (in >> str)
		{
			accuserInfo.number = atoi(str.c_str());
		}

		for (int i = 0; i < accuserInfo.number; ++i)
		{
			if (in >> str)
				(accuserInfo.vec_accused_name).push_back(str);
		}
		vec_accuserInfo.push_back(accuserInfo);
	}


	in.close();

	return true;
}

inline void DeleteElementFromList(const std::list<ACCUSER_INFO>::iterator iter, std::list<ACCUSER_INFO>& list_T)
{
	list_T.erase(iter);
}

bool AccusedCompareSet(const std::vector<std::string>& vec_accused, const std::set<std::string>& set)
{
	std::vector<std::string>::const_iterator iter;
	for(iter = vec_accused.begin(); iter != vec_accused.end(); ++iter)
	{
		if(set.find(*iter) != set.end())
			return true;
	}

	return false;
}

void LogicOpt(std::list<ACCUSER_INFO>& list_accuserInfo)
{
	/* 对任何一个举报人来说，不管他是诚实村的人还是谎言村的人，他举报的人一定是对方村的人 */

	/* 比如我们从第一个人Stephen开始，将举报人Stephen 和 被举报人Tommaso 分别放入 setA 和 setB,
	 * 然后依次将举报人和被举报人信息放入和Stephen和Tommaso有关联的集合中，可以确定的信息从vec_accuserInfo中删除，
	 * 不能确定的延迟确定，暂留在vec_accuserInfo中。等到信息熵达到时再做确定， 直到vec_accuserInfo为空，这时便可解题*/

	std::list<ACCUSER_INFO>::iterator iter;

	while (!list_accuserInfo.empty())
	{
		for (iter = list_accuserInfo.begin(); iter != list_accuserInfo.end(); ++iter)
		{
			/* 将第一份名单中的两方分别放入集合A，B*/
			if (setA_village.empty() && setB_village.empty())
			{
				setA_village.insert((*iter).accuser_name);
				for (unsigned int i = 0; i < (*iter).vec_accused_name.size(); ++i)
				{
					setB_village.insert((*iter).vec_accused_name.at(i));
				}

				DeleteElementFromList(iter, list_accuserInfo);
				break;
			}

			/* 循环执行举报着和已有集合中元素的对比，将被举报者放入对立集合 */
			if (setA_village.find((*iter).accuser_name) != setA_village.end())
			{
				for (unsigned int i = 0; i < (*iter).vec_accused_name.size(); ++i)
				{
					setB_village.insert((*iter).vec_accused_name.at(i));
				}
				DeleteElementFromList(iter, list_accuserInfo);
				break;
			}
			else if (setB_village.find((*iter).accuser_name)!= setB_village.end())
			{
				for (unsigned int i = 0; i < (*iter).vec_accused_name.size();++i)
				{
					setA_village.insert((*iter).vec_accused_name.at(i));
				}
				DeleteElementFromList(iter, list_accuserInfo);
				break;
			}

			/* 循环执行被举报者和已有集合中元素的对比，将举报者和被举报者放入对立集合*/
			if(AccusedCompareSet((*iter).vec_accused_name, setA_village))
			{
				for (unsigned int i = 0; i < (*iter).vec_accused_name.size();++i)
				{
					setA_village.insert((*iter).vec_accused_name.at(i));
				}
				setB_village.insert((*iter).accuser_name);
				DeleteElementFromList(iter, list_accuserInfo);
				break;
			}

			if (AccusedCompareSet((*iter).vec_accused_name, setB_village))
			{
				for (unsigned int i = 0; i < (*iter).vec_accused_name.size();++i)
				{
					setB_village.insert((*iter).vec_accused_name.at(i));
				}
				setA_village.insert((*iter).accuser_name);
				DeleteElementFromList(iter, list_accuserInfo);
				break;
			}

		}
	}
}
