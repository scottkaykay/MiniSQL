#include "interpreter.h"

SensefulStr::SensefulStr(std::string srcstr /*= ""*/)
	:src_str(srcstr)
{
	Parse();
}

void SensefulStr::SetSrcStr(std::string senstr)
{
	src_str = senstr;
	Parse();
}

std::vector<std::string> SensefulStr::GetSensefulStr() const
{
	return sen_str;
}

// ��������Ϊ�����ִ�
void SensefulStr::Parse()
{
	int i = 0;
	std::string token;
	while (i < src_str.size())
	{
		// ���жϱ�ʶ��
		if (src_str[i] == ' ')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();
			i++;
			continue;
		}
		if (src_str[i] == '\n')
		{
			i++;
			continue;
		}

		else if (src_str[i] == ',' || src_str[i] == '(' || src_str[i] == ')' || src_str[i] == '=')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();

			sen_str.push_back(std::string()+src_str[i]);
			i++;
			continue;
		}
		else if (src_str[i] == ';')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();

			sen_str.push_back(";");
			break;
		}

		token += src_str[i++];
	}
}

void GenerateParameterObj(std::vector<std::string> sen_str)
{
	auto tmp = sen_str.front();
	for (auto &c : tmp)
		tolower(c);

	if (tmp == "create")
	{
		auto second = sen_str[1];
		for (auto &c : second)
			tolower(c);

		if (second == "table")
		{
			CreateTableInfo(sen_str);
		}
	}
}

TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str)
{
	TB_Create_Info tb_create_info;

	assert(sen_str.size()>=3);
	tb_create_info.table_name = sen_str[2];

	if (sen_str[3] != "(")
		throw SQLError::CMD_FORMAT_ERROR();

	int i = 4;
	while (i < sen_str.size())
	{
		if (sen_str[i] == ";")
			break;
		if(i+1>=sen_str.size())
			throw SQLError::CMD_FORMAT_ERROR();

		TB_Create_Info::ColumnInfo colmu_info;
		colmu_info.name = sen_str[i];
		colmu_info.type = StrConvertToEnumType(sen_str[i + 1]);
		colmu_info.isPrimary = false;

		if (sen_str[i + 2] == ",")
		{
			tb_create_info.columns_info.push_back(colmu_info);
			i += 3;
			continue;
		}
		else if (sen_str[i + 2] == "primary")
		{
			colmu_info.isPrimary = true;
			tb_create_info.columns_info.push_back(colmu_info);
			i += 4;
			continue;
		}
		else if (sen_str[i + 2] == "(")
		{
			colmu_info.length = StrToInt(sen_str[i + 3]);
			if (sen_str[i + 4] != ")")
				throw SQLError::CMD_FORMAT_ERROR();
			if (sen_str[i + 5] == "primary")
			{
				colmu_info.isPrimary = true;
				tb_create_info.columns_info.push_back(colmu_info);
				i += 7;
				continue;
			}
			else
			{
				tb_create_info.columns_info.push_back(colmu_info);
				i += 6;
				continue;
			}
		}
	}

	return tb_create_info;
}

TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str)
{
	TB_Insert_Info tb_insert_info;


	if (StrToLower(sen_str[0]) != "insert" || StrToLower(sen_str[1]) != "into")
		throw SQLError::CMD_FORMAT_ERROR();

	// ��ȡ����
	tb_insert_info.table_name = sen_str[2];
	
	// �����﷨���
	int col_name_left_bracket = 3;  // �ֶ������б�������
	int col_name_right_bracket = 0;  // �ֶ������б�������
	int col_value_left_bracket = 0;  // �ֶ�ֵ�б�������
	int col_value_right_bracket = sen_str.size() - 2;  // �ֶ�ֵ�б�������

	if (sen_str[sen_str.size() - 1] != ";")
		throw SQLError::CMD_FORMAT_ERROR("Lack ';' at the end of command");

	for (int j = 3; j <sen_str.size()-1; j++)
	{
		if (sen_str[j] == ")")
		{
			col_name_right_bracket = j;
			col_value_left_bracket = j + 2;
			break;
		}
	}
	if(sen_str[col_value_left_bracket]!="("|| StrToLower(sen_str[col_name_right_bracket+1])!="values")
		throw SQLError::CMD_FORMAT_ERROR("key's count is not match value's count or values that not value");

	for (int p = col_name_left_bracket + 1, q = col_value_left_bracket + 1; q <= col_value_right_bracket - 1; p+=2, q+=2)
	{
		tb_insert_info.insert_info.push_back({ sen_str[p], sen_str[q] });
	}
#ifndef NDEBUG
	std::cout << "����ļ�¼��ֵ��" << std::endl;
	for (auto e : tb_insert_info.insert_info)
		std::cout << e.column_name << " " << e.column_value << std::endl;
#endif
	return tb_insert_info;
}