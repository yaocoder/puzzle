//============================================================================
// Name        : goodmorning.cpp
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
#include <iostream>
#include <fstream>

#define LOG_INFO(...) 	std::cout << "\033[01;34m " << __VA_ARGS__ << "\033[0m" << std::endl;
#define LOG_ERROR(...) 	std::cout << "\033[31m "	<< __VA_ARGS__ << "\033[0m" << std::endl;

static bool GetAppPath(std::string& app_path);
static void usage();
static bool ReadConf(const std::string& file_fullpath, int& number);
static void LogicOpt(const int number);

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

	/* 加载配置文件得到数字 */
	std::string file_fullpath = app_path + filename;
	int number = 0;
	if(!ReadConf(file_fullpath, number))
	{
		return EXIT_FAILURE;
	}

	if(15 != number)
	{
		LOG_INFO("WARNING : number is invalid number = " << number);
	}

	/* 逻辑输出 */
	LogicOpt(number);

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
		std::cout << "The App Path is : " << app_path << std::endl;
		return true;
	}

	return false;
}

void usage()
{
	LOG_INFO("Usage:");
	LOG_INFO("-f  [filename]   --  input filename must be 'my.conf'");
}

bool ReadConf(const std::string& file_fullpath, int& number)
{
	char buffer[10] = "\0";
	std::ifstream in(file_fullpath.c_str());
	if (!in.is_open())
	{
		LOG_ERROR("open file error. error =  " << strerror(errno));
		return false;
	}
	while (!in.eof())
	{
		in.getline(buffer, 10);
		in.close();
		break;
	}

	number = atoi(buffer);
	return true;
}

void LogicOpt(const int number)
{
	int mod = 0;
	for(int i = 1; i <=number; ++i)
	{
		mod = i%24;
		if(mod >= 7 && mod <= 9)
		{
			LOG_INFO("Good Moring");
		}
		else
		{
			LOG_INFO("hello");
		}
	}
}
