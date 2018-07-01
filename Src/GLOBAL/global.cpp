#include "global.h"

SensefulStr::SensefulStr(std::string srcstr /*= ""*/)
	:src_str(srcstr)
{
	Parse();
}

void SensefulStr::SetSrcStr(std::string _srcstr)
{
	src_str = _srcstr;
	sen_str.clear();
	Parse();
}

std::vector<std::string> SensefulStr::GetSensefulStr() const
{
	return sen_str;
}

// 解析命令为有意字串
void SensefulStr::Parse()
{
	int i = 0;
	std::string token;
	while (i < src_str.size())
	{
		// 先判断标识符
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

		else if (src_str[i] == ',' || src_str[i] == '(' || src_str[i] == ')')// || src_str[i] == '=')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();

			sen_str.push_back(std::string() + src_str[i]);
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

Column_Type StrConvertToEnumType(std::string str_type)
{
	for (auto &c : str_type)
		tolower(c);

	if (str_type == "int")
	{
		return Column_Type::I;
	}
	if (str_type == "char")
	{
		return Column_Type::C;
	}
	if (str_type == "double")
	{
		return Column_Type::D;
	}

	return Column_Type::I;
}

std::string IdxToDbf(std::string idx_name)
{
	std::string dbf_name(idx_name);

	int i = idx_name.size()-1;
	while (idx_name[i] != '.')
		i--;
	if (i < 0) throw SQLError::FILENAME_CONVERT_ERROR();
	idx_name[i + 1] = 'd';
	idx_name[i + 2] = 'b';
	idx_name[i + 3] = 'f';
	idx_name[i + 4] = '\0';

	return dbf_name;
}

std::string DbfToIdx(std::string dbf_name)
{
	std::string idx_name(dbf_name);

	int i = idx_name.size() - 1;
	while (idx_name[i] != '.')
		i--;
	if (i < 0) throw SQLError::FILENAME_CONVERT_ERROR();
	idx_name[i + 1] = 'i';
	idx_name[i + 2] = 'd';
	idx_name[i + 3] = 'x';
	idx_name[i + 4] = '\0';

	return idx_name;
}

int StrToInt(std::string str)
{
	int x = 0;
	for (int i = 0; i < str.size(); i++)
	{
		x = x * 10 + str[i] - '0';
	}
	return x;
}

std::string StrToLower(std::string str)
{
	for (auto &c : str)
		tolower(c);
	return str;
}



std::string IntToStr3(unsigned int x)
{
	std::string str = "000";
	str[2] = (x % 10) + '0';
	str[0] = (x / 100) + '0';
	str[1] = (x % 100)/10 + '0';
	return str;
}

CatalogPosition& GetCp()
{
	static CatalogPosition cp;
	return cp;
}

bool CatalogPosition::isInSpeDb = false;

CatalogPosition::CatalogPosition()
	:root("./DB/"), current_catalog("./DB/")
{
	// 如果当前目录下没有 DB 文件见则创建
	std::string tmp_path = "./DB";

	if (_access(tmp_path.c_str(), 0) == -1)
	{
		_mkdir(tmp_path.c_str());
	}
}

bool CatalogPosition::ResetRootCatalog(std::string root_new)
{
	if (root_new[root_new.size() - 1] == '/')
	{
		root = root_new;
		current_catalog = root;
		isInSpeDb = false;
		return true;
	}
	else
	{
		return false;
	}
}

void CatalogPosition::SwitchToDatabase()
{
	current_catalog = root;
	isInSpeDb = false;
}

bool CatalogPosition::SwitchToDatabase(std::string db_name)
{
	std::string tmp_path = root + db_name;

	if (_access(tmp_path.c_str(), 0) == -1)  //判断数据库是否存在
	{
		return false;
	}
	else
	{
		current_catalog = root + db_name + "/";
		isInSpeDb = true;
		return true;
	}

}

std::string CatalogPosition::GetCurrentPath() const
{
	return current_catalog;
}

std::string CatalogPosition::GetRootPath() const
{
	return root;
}

std::string CatalogPosition::SetCurrentPath(std::string cur)
{
	current_catalog = cur;
	return current_catalog;
}


bool KeyAttr::operator<(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = this->value.IntValue < rhs.value.IntValue;
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = s1 < s2;
		break;

	case Column_Type::D:
		res = this->value.DoubleValue < rhs.value.DoubleValue;
		break;
	default:
		break;
	}
	return res;
}

bool KeyAttr::operator>(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = this->value.IntValue > rhs.value.IntValue;
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = s1 > s2;
		break;

	case Column_Type::D:
		res = this->value.DoubleValue > rhs.value.DoubleValue;
		break;
	default:
		break;
	}
	return res;
}

bool KeyAttr::operator>=(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	return !(*this < rhs);
}

bool KeyAttr::operator!=(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	return !(*this == rhs);
}

bool KeyAttr::operator<=(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	return !(*this > rhs);
}

bool KeyAttr::operator==(const KeyAttr &rhs)const
{
	if (this->type != rhs.type)
		return false;

	bool res = true;
	std::string s1;
	std::string s2;

	switch (this->type)
	{
	case Column_Type::I:
		res = (this->value.IntValue == rhs.value.IntValue);
		break;

	case Column_Type::C:
		s1 = std::string(this->value.StrValue);
		s2 = std::string(rhs.value.StrValue);
		res = (s1 == s2);
		break;

	case Column_Type::D:
		res = (this->value.DoubleValue == rhs.value.DoubleValue);
		break;
	default:
		break;
	}
	return res;
}

std::ostream& operator<<(std::ostream &os, const KeyAttr &key)
{
	switch (key.type)
	{
	case Column_Type::I:
		os << key.value.IntValue << " ";
		break;

	case Column_Type::C:
		os << key.value.StrValue << " ";
		break;

	case Column_Type::D:
		os << key.value.DoubleValue << " ";
		break;
	default:
		break;
	}

	return os;
}

void FileAddr::SetFileAddr(const unsigned long _filePageID /*= 0*/, const unsigned int _offSet /*= 0*/)
{
	filePageID = _filePageID;
	offSet = _offSet;
}
void FileAddr::ShiftOffset(const int OFFSET)
{
	this->offSet += OFFSET;
}